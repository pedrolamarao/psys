// Copyright (C) 2012,2013,2014,2015,2016,2021,2022 Pedro Lamarão <pedro.lamarao@gmail.com>. All rights reserved.

#pragma once

#include <x86/common.h>
#include <x86/registers.h>

#include <x86/_32/segments.h>
#include <x86/_64/segments.h>


// Interface.

namespace x86
{
  //! Types.
  //! @{

  //! Code segment descriptor type.

  constexpr
  auto code_segment ( bool conforming, bool readable, bool accessed ) -> descriptor_type ;

  //! Data segment descriptor type.

  constexpr
  auto data_segment ( bool downwards, bool writable, bool accessed ) -> descriptor_type ;

  //! @}

  //! Operators.
  //! @{

  //! Gets the global descriptor table register (GDTR).

  auto get_global_descriptor_table () -> descriptor_table ;

  //! Sets the global descriptor table register (GDTR).

  void set_global_descriptor_table ( descriptor_table value );

  //! Get code segment register (CS).

  auto get_code_segment () -> segment_selector ;

  //! Set code segment register (CS).

  void set_code_segment ( segment_selector value );

  //! Set data segment register (DS).

  void set_data_segment ( segment_selector value );

  //! Set data segment registers (DS, ES, FS, GS, SS).

  void set_data_segments ( segment_selector value );

  //! Set stack segment register (SS).

  void set_stack_segment ( segment_selector value );

  //! @}
}

// Implementation.

namespace x86
{
  constexpr inline
  auto code_segment ( bool conforming, bool readable, bool accessed ) -> descriptor_type {
    return (1 << 3) | (conforming << 2) | (readable << 1) | accessed;
  }

  constexpr inline
  auto data_segment ( bool downwards, bool writable, bool accessed ) -> descriptor_type {
    return (0 << 3) | (downwards << 2) | (writable << 1) | accessed;
  }

  inline
  auto get_global_descriptor_table () -> descriptor_table
  {
    return gdtr();
  }

  inline
  void set_global_descriptor_table ( descriptor_table value )
  {
    gdtr(value);
  }
}