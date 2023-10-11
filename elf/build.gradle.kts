plugins {
    id("br.dev.pedrolamarao.metal.archive")
    id("br.dev.pedrolamarao.metal.cpp")
    id("br.dev.pedrolamarao.metal.cxx")
    id("br.dev.pedrolamarao.metal.ixx")
}

group = "br.dev.pedrolamarao.metal.elf"

dependencies {
    api(project(":psys"))
}

metal {
    compileOptions = listOf("-fasm-blocks","-g","-std=c++20","-Wno-unused-command-line-argument")

    ixx {
        main {
            public = true
        }
    }
}

// TODO: enhance Gradle Metal with test dependencies

dependencies {
    implementation(project(":googletest"))
}

// TODO: enhance Gradle Metal with component target selector/filter

afterEvaluate {
    val targets = listOf("x86_64-pc-linux-gnu","x86_64-pc-windows-msvc")
    val targetEnabled = targets.contains(metal.target.get())
    tasks.named("compile-test-cxx") { enabled = targetEnabled }
    tasks.named("link-test") { enabled = targetEnabled }
    tasks.named("run-test") { enabled = targetEnabled }
    tasks.named("test") { enabled = targetEnabled }
}