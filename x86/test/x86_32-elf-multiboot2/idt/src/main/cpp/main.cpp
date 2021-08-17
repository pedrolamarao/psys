// Copyright (C) 2020,2021 Pedro Lamarão <pedro.lamarao@gmail.com>. All rights reserved.


#include <psys/integer.h>

#include <multiboot2/header.h>
#include <multiboot2/information.h>

#include <x86/gdt.h>
#include <x86/idt.h>

// Multiboot2 boot request

namespace multiboot2
{
    using namespace multiboot2;

    struct request_type
    {
        header_prologue prologue;
        end_request     end;
    };

    [[gnu::used, gnu::section(".multiboot2")]]
    constexpr request_type request =
    {
        { architecture_type::x86, sizeof(request), },
        { },
    };
}

// IA32 GDT

namespace x86
{
    [[gnu::section(".gdt")]]
    constexpr segment_descriptor global_descriptor_table [6] =
    {
        // required null descriptor
        { },
        // unexpected null descriptor!
        { },
        // system flat code descriptor
        { 0, 0xFFFFF, code_segment(true, true, true), 0, true, true, true, true, },
        // system flat data descriptor
        { 0, 0xFFFFF, data_segment(true, true, true), 0, true, true, true, true, },
        // user flat code descriptor
        { 0, 0xFFFFF, code_segment(true, true, true), 3, true, true, true, true, },
        // user flat data descriptor
        { 0, 0xFFFFF, data_segment(true, true, true), 3, true, true, true, true, },
    };

    void set_segment_registers ( segment_selector code, segment_selector data )
    {
        set_code_segment_register(code);
        set_data_segment_registers(data);
    }
}

// IA32 IDT

namespace x86
{
    constexpr unsigned interrupt_descriptor_table_size = 256;

    [[gnu::section(".idt")]]
    interrupt_gate_descriptor interrupt_descriptor_table [ interrupt_descriptor_table_size ];

    extern "C"
    [[gnu::used]]
    unsigned volatile interrupted {};

    extern "C"
    void __interrupt_service_routine ();
}

//! Psys test protocol

extern "C"
{
    [[gnu::used]] unsigned volatile _test_control {};

    [[gnu::used]] unsigned volatile _test_debug {};
}

//! Multiboot2 entry point

extern "C"
void main ( ps::size4 magic, multiboot2::information_list & mbi )
{
    using namespace multiboot2;
    using namespace ps;
    using namespace x86;

    // test: boot sanity

    _test_control = 1;

    if (magic != multiboot2::information_magic) {
        _test_control = 0;
        return;
    }

    // set the GDT register and set segment registers

    _test_control = 2;

    set_global_descriptor_table_register(global_descriptor_table);

    set_segment_registers(segment_selector(2, false, 0), segment_selector(3, false, 0));

    // test: data structures

    auto interrupt_segment = segment_selector(2, false, 0);

    for (auto i = 0U, j = 256U; i != j; ++i) {
        interrupt_descriptor_table[i] = { interrupt_segment, __interrupt_service_routine, true, true, 0, true };
    }

    auto& descriptor = interrupt_descriptor_table[0];

    _test_control = 10;
    if (descriptor.type() != 0b1110) {
        _test_debug = descriptor.type();
        _test_control = 0;
        return;
    }

    _test_control = 11;
    if (descriptor.privilege() != 0) {
        _test_debug = descriptor.privilege();
        _test_control = 0;
        return;
    }

    _test_control = 12;
    if (descriptor.is_present() != true) {
        _test_control = 0;
        return;
    }

    _test_control = 13;
    if (descriptor.segment() != interrupt_segment) {
        _test_debug = size2{descriptor.segment()};
        _test_control = 0;
        return;
    }

    _test_control = 14;
    if (descriptor.offset() != size4(__interrupt_service_routine)) {
        _test_debug = descriptor.offset();
        _test_control = 0;
        return;
    }

    _test_control = 15;
    if (descriptor.is_32bit() != true) {
        _test_control = 0;
        return;
    }

    _test_control = 16;
    if (descriptor.must_cli() != true) {
        _test_control = 0;
        return;
    }

    // set the IDT register

    _test_control = 20;

    set_interrupt_descriptor_table_register(interrupt_descriptor_table);
    
    // test: did we successfully update the IDT register?

    _test_control = 21;

    auto expected_idtr = system_table_register {
        interrupt_descriptor_table_size * sizeof(interrupt_gate_descriptor),
        reinterpret_cast<size4>(interrupt_descriptor_table)
    };

    auto actual_idtr = get_interrupt_descriptor_table_register();

    if (actual_idtr != expected_idtr) {
        _test_debug = expected_idtr.size;
        _test_debug = expected_idtr.offset;
        _test_debug = actual_idtr.size;
        _test_debug = actual_idtr.offset;
        _test_control = 0;
        return;
    }

    // test: handle one software interrupt

    _test_control = 30;

    interrupt<0x30>();

    if (interrupted != 1) {
        _test_control = 0;
        return;
    }

    // test: handle another software interrupt

    _test_control = 31;

    interrupt<0x31>();

    if (interrupted != 2) {
        _test_control = 0;
        return;
    }

    // test: enable hardware interrupts

    _test_control = 40;

    enable_interrupts();

    _test_control = -1;
    return;
}
