// Copyright (C) 2020, 2021 Pedro Lamarão <pedro.lamarao@gmail.com>. All rights reserved.


#include <multiboot2/header.h>
#include <multiboot2/information.h>

#include <psys/test.h>


//! Test.

namespace
{
    //! Uninteresting test procedure.
    //! Did we correctly define the Multiboot2 entry point?

    [[gnu::used]]
    void test ()
    {
        _test_control = 1;
        _test_control = -1;
    }

    //! Very large object in the text section.
    //! Did we correctly position the Multiboot2 request object?

    [[gnu::used]]
    void large_text ()
    {
        __asm__
        {
            .zero 0x8000
        }
    }

    //! Very large object in the data section.
    //! Did we correctly position the Multiboot2 request object?

    [[gnu::used]]
    constinit
    char large_data [ 0x8000 ] { -1 };
}

//! Multiboot 2 loader.

namespace multiboot2
{
    //! Multiboot2 request.

    struct request_type
    {
        header_prologue        prologue;
        entry_address_request  address;
        end_request            end;
    };

    // Assumption: multiboot2_start is located at physical address 0x1000.
    //! Did we correctly position the Multiboot2 entry point?

    [[gnu::used, gnu::section(".multiboot2.request")]]
    constinit
    request_type request =
    {
        { architecture_type::x86, sizeof(request), },
        { tag_type::entry, 0, sizeof(entry_address_request), 0x1000 },
        { },
    };

    //! Multiboot2 entry point stack.

    [[gnu::section(".multiboot2.stack")]]
    constinit
    unsigned char stack [ 0x4000 ] {};

    //! Multiboot2 entry point.

    extern "C"
    [[gnu::naked, gnu::section(".multiboot2.start")]]
    void multiboot2_start ()
    {
        __asm__
        {
#if defined(__i386__)
            mov esp, offset stack + 0x4000
            xor ecx, ecx
            push ecx
            popf
#elif defined(__x86_64__)
            mov rsp, offset stack + 0x4000
            xor rcx, rcx
            push rcx
            popf
#else
# error unsupported target
#endif
            call _test_start
            call test
            call _test_finish
            __multiboot2_halt:
            hlt
            jmp __multiboot2_halt
        }
    }
}