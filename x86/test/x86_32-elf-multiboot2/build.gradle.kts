import dev.nokee.platform.cpp.CppApplication
import dev.nokee.platform.nativebase.ExecutableBinary

plugins {
    base
}

subprojects {
    apply(plugin = "psys-test")

    project.extensions.configure<CppApplication> {
        targetMachines.addAll(
            // #XXX: build on any for x86_32-elf-multiboot2
            machines.linux.architecture("-multiboot-x86_32"),
            machines.windows.architecture("-multiboot-x86_32"),
            // #XXX: build on any for x86_32-elf-multiboot2
            machines.linux.architecture("-multiboot-x86_64"),
            machines.windows.architecture("-multiboot-x86_64"),
        )

        dependencies {
            implementation(project(":psys:multiboot2"))
            implementation(project(":x86"))
        }

        val baseArgs = listOf("-std=c++20", "-flto", "-fasm-blocks")

        binaries.configureEach {
            if (this is ExecutableBinary) {
                compileTasks.configureEach {
                    compilerArgs.addAll(baseArgs)
                }
                linkTask {
                    linkerArgs.addAll(baseArgs)
                }
            }
        }
    }
}
