plugins {
    id("be.seeseemelk.raviolios.cppsource")
    id("be.seeseemelk.raviolios.java")
}

dependencies {
    implementation("raviolios:lib.directtoc")
}

group = "raviolios"

val outputC = layout.buildDirectory.dir("generated_c").get().toString()
val outputH = layout.buildDirectory.dir("generated_h").get().toString()

tasks.register<JavaExec>("generateC") {
    dependsOn("compileJava")

    inputs.files(sourceSets.getByName("main").allSource)

    mainClass.set("be.seeseemelk.kernel.core.Core")
    args = listOf(outputC, outputH)
    classpath = sourceSets.getByName("main").runtimeClasspath
}
