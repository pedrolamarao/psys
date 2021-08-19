// Copyright (C) 2020 Pedro Lamarão <pedro.lamarao@gmail.com>. All rights reserved.


#include <multiboot2/header.h>
#include <multiboot2/information.h>


//! Psys test protocol

extern "C"
{
	[[gnu::used]] unsigned volatile _test_control {};
	[[gnu::used]] unsigned volatile _test_debug {};

    void _test_start () { }
    void _test_finish () { }
}

//! Multiboot 2 loader.

namespace
{
    using namespace multiboot2;

    struct request_type
    {
        header_prologue prologue;
        end_request     end;
    };

    [[gnu::used, gnu::section(".text")]]
    constinit
    request_type request =
    {
        { architecture_type::x86, sizeof(request), },
        { },
    };

    constinit
    unsigned char multiboot2_stack [ 0x4000 ] {};

    extern "C"
    [[gnu::naked]]
    void __multiboot2_start ()
    {
        __asm__
        {
            mov esp, offset multiboot2_stack + 0x4000
            xor ecx, ecx
            push ecx
            popf
            call _test_start
            mov _test_control, 1
            mov _test_control, -1
            call _test_finish
            __multiboot2_halt:
            hlt
            jmp __multiboot2_halt
        }
    }
}