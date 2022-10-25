plugins {
    id("be.seeseemelk.raviolios.cpp")
    id("be.seeseemelk.raviolios.java")
    `java-library`
}

group = "raviolios"

repositories {
    mavenCentral()
}

dependencies {
    implementation("raviolios:lib.directtoc")
    implementation("raviolios:kernel.core")
    implementation("raviolios:kit.java")
}

val outputC = layout.buildDirectory.dir("generated/c").get().toString()
val outputH = layout.buildDirectory.dir("generated/h").get().toString()

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
    extraSourceDirs.from(outputC)
    extraHeaderDirs.from(outputH)
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

task("generateC", JavaExec::class) {
    dependsOn("compileJava")

    inputs.files(sourceSets.getByName("main").allSource)
    outputs.files(File(outputC, "d2c_raviolios.c"), File(outputH, "d2c_raviolios.h"))

    mainClass.set("raviolios.kernel.test.Generator")
    args = listOf(outputC, outputH)
    classpath = sourceSets.getByName("main").runtimeClasspath
}

tasks.getByName("compileCpp").dependsOn("generateC");
