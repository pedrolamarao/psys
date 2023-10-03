import org.ajoberstar.grgit.Grgit

plugins {
    id("br.dev.pedrolamarao.metal.cpp")
    id("br.dev.pedrolamarao.metal.prebuilt")
    id("org.ajoberstar.grgit") version("5.2.0") apply(false)
}

val source = layout.buildDirectory.dir("src").get()
val build = layout.buildDirectory.dir("obj").get()

val clone = tasks.register("clone") {
    doLast {
        if (! source.asFile.exists()) {
            Grgit.clone {
                depth = 1
                dir = source
                uri = "https://github.com/google/googletest"
            }
        }
    }
}

val configure = tasks.register<Exec>("configure") {
    dependsOn(clone)
    executable("cmake")
    args(
        "-B",build,
        "-DCMAKE_BUILD_TYPE=Release",
        "-DCMAKE_TOOLCHAIN_FILE=${projectDir}/llvm.cmake",
        "-G","Ninja",
        "-S",source
    )
}

val make = tasks.register<Exec>("make") {
    dependsOn(configure)
    executable("cmake")
    args("--build",build)
}

metal {
    prebuilt {
        includable(source.dir("googletest/include")) { builtBy(clone) }
        linkable(build.file("lib/gtest.lib")) { builtBy(make) }
    }
}