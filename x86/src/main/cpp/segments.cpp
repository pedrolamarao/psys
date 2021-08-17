// Copyright (C) 2021 Pedro Lamarão <pedro.lamarao@gmail.com>. All rights reserved.

#include <x86/gdt.h>

namespace x86
{
    // Interface types.

    system_table_register get_global_descriptor_table_register ()
    {
        system_table_register value;
        __asm__ ( "sgdtl %0" : "=m"(value) : : );
        return value;
    }

    void set_global_descriptor_table_register ( system_table_register value )
    {
        __asm__ ( "lgdtl %0" : : "m"(value) : );
    }

    auto get_code_segment_register () -> segment_selector
    {
        segment_selector value;
        __asm__ ( "movw %0, %%cs" : "=m"(value) );
        return value;
    }

    void set_code_segment_register ( segment_selector value )
    {
        __asm__ (
            "pushl %0        \n"
            "movl %%eax, %=f \n"
            "pushl %%eax     \n"
            "lretl           \n"
            "%=:             \n"
            :
            : "m"(value)
            :
       );
    }

    void set_data_segment_register ( segment_selector value )
    {
        __asm__ ( "movw %%ds, %0" : : "m"(value) : );
    }

    void set_stack_segment_register ( segment_selector value )
    {
        __asm__ ( "movw %%ss, %0" : : "m"(value) : );
    }

    void set_extra_segment_registers ( segment_selector value )
    {
        __asm__ ( "movw %%ds, %0" : : "m"(value) : );
    }
}