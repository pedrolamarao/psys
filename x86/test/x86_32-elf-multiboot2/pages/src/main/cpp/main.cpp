// Copyright (C) 2022 Pedro Lamarão <pedro.lamarao@gmail.com>. All rights reserved.


#include <psys/start.h>
#include <psys/test.h>

#include <x86/interrupts.h>
#include <x86/pages.h>
#include <x86/segments.h>


namespace
{
    using namespace x86;
    using namespace x86::_32;

    // segments.

    extern
    segment_descriptor global_descriptor_table [ 6 ];

    // interrupts.

    extern
    interrupt_gate_descriptor interrupt_descriptor_table [ 256 ];

    void interrupt_handler ();

    // pages.

    // short small paging.

    alignas(0x1000) constinit
    short_page_entry short_page_table [ 0x400 ] {};

    alignas(0x1000) constinit
    short_small_page_directory_entry short_small_page_directory_table [ 0x400 ] {};

    // short large paging.

    alignas(0x1000) constinit
    short_large_page_directory_entry short_large_page_directory_table [ 0x400 ] {};

    // long small paging.

    alignas(0x1000) constinit
    long_page_entry long_page_table [ 0x200 ] {};

    alignas(0x1000) constinit
    long_small_page_directory_entry long_small_page_directory_table [ 0x200 ] {};

    alignas(0x1000) constinit
    page_directory_pointer_entry small_page_directory_pointer_table [ 0x200 ] {};

    // long large paging.

    alignas(0x1000) constinit
    long_large_page_directory_entry long_large_page_directory_table [ 0x200 ] {};

    alignas(0x1000) constinit
    page_directory_pointer_entry large_page_directory_pointer_table [ 0x200 ] {};
}

void psys::main ()
{
    using namespace x86;
    using namespace x86::_32;
    
    size step = 1;

    // prepare segments.

    _test_control = step++;
    set_global_descriptor_table(global_descriptor_table);
    set_code_segment( segment_selector(2, false, 0) );
    set_data_segments( segment_selector(3, false, 0) );

    // prepare interrupts.

    _test_control = step++;
    auto interrupt_segment = segment_selector(2, false, 0);
    for (auto i = 0U, j = 256U; i != j; ++i) {
        interrupt_descriptor_table[i] = { interrupt_segment, interrupt_handler, true, true, 0, true };
    }
    set_interrupt_descriptor_table(interrupt_descriptor_table);

    // Multiboot2 x86 requires that paging is disabled at entry time.

    _test_control = step++;
    if (is_paging()) {
        _test_control = 0;
        return;
    }

    // Verify we can manipulate control registers.

    // Verify we can manipulate page size extensions (CR4.PSE).

    step = 100;

    _test_control = step++;
    disable_large_pages();

    _test_control = step++;
    if (is_large_pages()) {
        _test_control = 0;
        return;
    }

    _test_control = step++;
    enable_large_pages();

    _test_control = step++;
    if (! is_large_pages()) {
        _test_control = 0;
        return;
    }

    _test_control = step++;
    disable_large_pages();

    _test_control = step++;
    if (is_large_pages()) {
        _test_control = 0;
        return;
    }

    // Post: page size extensions (CR4.PSE) disabled.

    // Verify we can manipulate page address extensions (CR4.PAE).

    step = 200;

    _test_control = step++;
    disable_long_addresses();

    _test_control = step++;
    if (is_long_addresses()) {
        _test_control = 0;
        return;
    }

    _test_control = step++;
    enable_long_addresses();

    _test_control = step++;
    if (! is_long_addresses()) {
        _test_control = 0;
        return;
    }

    _test_control = step++;
    disable_long_addresses();

    _test_control = step++;
    if (is_long_addresses()) {
        _test_control = 0;
        return;
    }

    // Post: page address extensions (CR4.PAE) disabled.

    // Verify we can manipulate the paging control (CR3) register.

    step = 300;

    _test_control = step++;
    get_short_paging();

    _test_control = step++;
    get_long_paging();

    _test_control = step++;
    set_paging( short_paging { 0, 0, 0 } );

    _test_control = step++;
    auto control = get_short_paging();

    _test_control = step++;
    if (control.write_through() != 0) {
        _test_control = 0;
        return;
    }

    _test_control = step++;
    if (control.cache() != 0) {
        _test_control = 0;
        return;
    }

    _test_control = step++;
    if (control.address() != 0) {
        _test_control = 0;
        return;
    }

    _test_control = step++;
    set_paging( short_paging { 1, 1, 1 } );

    _test_control = step++;
    control = get_short_paging();

    _test_control = step++;
    if (control.write_through() != 1) {
        _test_control = 0;
        return;
    }

    _test_control = step++;
    if (control.cache() != 1) {
        _test_control = 0;
        return;
    }

    _test_control = step++;
    if (control.address() != 0x1000) {
        _test_control = 0;
        return;
    }

    // Post: paging control (CR3) register is invalid!

    // Verify that we can do short small paging.

    step = 400;

    for (size i = 0; i != 0x400; ++i) {
        short_page_table[i] = {
            {}, true, true, true, false, false, false, false, 0, false, 0, (0x1000 * i)
        };
    }

    for (size i = 0; i != 0x400; ++i) {
        short_small_page_directory_table[i] = {
            {}, true, true, true, false, false, false, 0, reinterpret_cast<size4>(short_page_table)
        };
    }

    _test_control = step++;
    disable_large_pages();

    _test_control = step++;
    disable_long_addresses();

    _test_control = step++;
    set_paging( short_paging { {}, false, false, reinterpret_cast<size4>(short_small_page_directory_table) } );

    _test_control = step++;
    enable_paging();

    _test_control = step++;
    if (! is_paging()) {
        _test_control = 0;
        return;
    }

    _test_control = step++;
    disable_paging();

    // Verify that we can do short large paging.

    step = 500;

    for (size i = 0; i != 0x400; ++i) {
        short_large_page_directory_table[i] = {
            true, true, true, false, false, false, false, false, 0, 0, (0x400000 * i)
        };
    }

    _test_control = step++;
    enable_large_pages();

    _test_control = step++;
    disable_long_addresses();

    _test_control = step++;
    set_paging( short_paging { {}, false, false, reinterpret_cast<size4>(short_large_page_directory_table) } );

    _test_control = step++;
    enable_paging();

    _test_control = step++;
    if (! is_paging()) {
        _test_control = 0;
        return;
    }

    _test_control = step++;
    disable_paging();

    _test_control = -1;
    return;
}

namespace
{
    // segments.

    constexpr
    unsigned global_descriptor_table_size { 6 };

    constinit
    segment_descriptor global_descriptor_table [ global_descriptor_table_size ] =
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

    // interrupts.

    constinit
    interrupt_gate_descriptor interrupt_descriptor_table [ 256 ]
    { };

    [[gnu::naked]]
    void interrupt_handler ()
    {
        __asm__
        {
            cli
        loop:
            hlt
            jmp loop
        }
    }

    // pages.

    alignas(0x1000) constinit
    short_page_entry page_table [ 0x400 ] {};

    alignas(0x1000) constinit
    short_small_page_directory_entry page_directory_table [ 0x400 ] {};
}