plugins {
    `java-gradle-plugin`
    id("io.freefair.lombok") version "6.5.1"
}

repositories {
    mavenCentral()
    gradlePluginPortal()
}

java {
    toolchain {
        languageVersion.set(JavaLanguageVersion.of(17))
    }
}

gradlePlugin {
    val `raviolios-java` by plugins.creating {
        id = "be.seeseemelk.raviolios.java"
        implementationClass = "be.seeseemelk.raviolios.plugin.java.JavaPlugin"
    }
}

dependencies {
    implementation("io.freefair.gradle:lombok-plugin:6.5.1")
}
