// Copyright (C) 2022 Pedro Lamarão <pedro.lamarao@gmail.com>. All rights reserved.

#pragma once

#include <x86/common.h>


// Interface.

namespace x86
{
    //! Processor identification.

    struct cpuid_type { size a {}; size b {}; size c {}; size d {}; };

    //! Get processor identification.

    auto cpuid2 (size feature, size variant = 0) -> cpuid_type;

    //! Halt processor.

    void halt ();

    //! Read from I/O port.

    auto in1 ( size2 port ) -> size1;

    //! Read from I/O port.

    auto in2 ( size2 port ) -> size2;

    //! Read from I/O port.

    auto in4 ( size2 port ) -> size4;

    //! Write to I/O port.

    void out1 ( size2 port, size1 data );

    //! Write to I/O port.

    void out2 ( size2 port, size2 data );

    //! Write to I/O port.

    void out4 ( size2 port, size4 data );

    //! Pause processor.

    void pause ();
}