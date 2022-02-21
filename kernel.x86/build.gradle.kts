plugins {
    id("be.seeseemelk.raviolios.cpp")
}

group = "kernel"

cpp {
    dependency("kernel.core")
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
