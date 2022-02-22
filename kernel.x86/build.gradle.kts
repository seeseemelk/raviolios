plugins {
    id("be.seeseemelk.raviolios.cpp")
    id("be.seeseemelk.raviolios.java")
}

group = "kernel"

cpp {
    dependency("kernel.core")
    linkerScript.set("kernel.ld")
    commonFlags.addAll(
            "--target=i386-elf",
            "-march=i386",
            "-Wall",
            "-Wextra",
            "-Werror",
            "-ffreestanding",
            "-ggdb",
            "-fno-builtin",
            "-fno-c++-static-destructors",
    )
    cppFlags.addAll(
            "-fno-rtti",
            "-fno-exceptions",
            "--std=c++17"
    )
    ldFlags.addAll(
            "-nostdlib",
            "-lgcc"
    )
}

tasks.register<Exec>("emulate") {
    group = "application"
    description = "Runs the kernel under Qemu"
    //dependsOn(buildIso)
    workingDir(rootDir)
    commandLine("qemu-system-i386", "-cdrom", "isoFile", "-m", "16M", "-gdb", "tcp::1234", "-serial", "stdio")
}
