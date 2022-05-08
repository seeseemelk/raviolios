plugins {
    id("be.seeseemelk.raviolios.cpp")
    id("be.seeseemelk.raviolios.java")
    `java-library`
}

group = "raviolios"

dependencies {
    implementation("raviolios:kernel.core")
    implementation("raviolios:kit.java")
}

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

tasks.check {
    dependsOn("testCpp")
}
