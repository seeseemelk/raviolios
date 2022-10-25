plugins {
    id("be.seeseemelk.raviolios.cpp")
    id("be.seeseemelk.raviolios.java")
    id("be.seeseemelk.raviolios.icb")
}

group = "kernel"

repositories {
    mavenCentral()
}

dependencies {
    implementation("raviolios:kernel.core")
    implementation("raviolios:kit.core")
}

cpp {
	useDocker.set(false)
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
            "-nostdlib"
    )
}

val isoDir = File(buildDir, "isoroot")
val isoFile = File(buildDir, "iso/raviolios-x86.iso")

tasks.register<Exec>("emulate") {
    description = "Boots the ISO under Qemu"
    group = "application"

    dependsOn("iso")
    commandLine("qemu-system-i386", "-cdrom", isoFile, "-m", "3M", "-gdb", "tcp::1234", "-serial", "stdio")
}

tasks.register<Exec>("debug") {
    description = "Boots the ISO under Qemu"
    group = "application"

    dependsOn("iso")
    commandLine("qemu-system-i386", "-cdrom", isoFile, "-m", "3M", "-gdb", "tcp::1234", "-serial", "stdio", "-S")
}

tasks.register<Exec>("iso") {
    description = "Generates the ISO file"
    group = "build"
    dependsOn("prepareIso")

    doFirst {
        isoFile.parentFile.mkdirs();
    }

    workingDir(rootDir)
    commandLine("grub-mkrescue", "-o", isoFile, isoDir)
}

tasks.register<Copy>("prepareIso") {
    description = "Generates the ISO root directory"
    dependsOn("icb", "assemble")

    from(file("src/main/resources/isodir"))
    from(tasks.getByName("icb").outputs) {
        rename(".+\\.icb$", "boot/raviolios.icb")
    }
    from(File(buildDir, "cpp/output.elf")) {
        rename(".+\\.elf$", "boot/raviolios-i386.elf")
    }
    into(isoDir)
}
