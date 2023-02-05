plugins {
    id("be.seeseemelk.raviolios.cppsource")
    id("be.seeseemelk.raviolios.java")
}

repositories {
    mavenCentral()
}

dependencies {
    implementation("raviolios:lib.directtoc")
}

tasks.register<JavaExec>("generate") {
    dependsOn.add("build")
    classpath = sourceSets.main.get().runtimeClasspath
    mainClass.set("raviolios.directtoc.core.Main")
    args = listOf("build/generated/cpp")
}

group = "raviolios"
