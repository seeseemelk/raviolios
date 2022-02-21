plugins {
    id("be.seeseemelk.raviolios.cpp")
    `java-library`
}

group = "kernel"

cpp {
    dependency("kernel.core")
    commonFlags.addAll(
            "-Wall",
            "-Wextra",
            "-Werror",
            "-ggdb",
            "-fsanitize=address",
    )
    cppFlags.addAll(
            "--std=c++17",
    )
}

task("testCpp", Exec::class) {
    dependsOn("assemble")
    setCommandLine("build/cpp/output.elf")
    group = "verification"
    description = "Runs C/C++ test binary"
}

tasks.check.configure {
    dependsOn("testCpp")
}
