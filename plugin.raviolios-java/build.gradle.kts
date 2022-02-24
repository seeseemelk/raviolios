plugins {
    `java-gradle-plugin`
}

repositories {
    mavenCentral()
}

java {
    toolchain {
        languageVersion.set(JavaLanguageVersion.of(11))
    }
}

gradlePlugin {
    val `raviolios-java` by plugins.creating {
        id = "be.seeseemelk.raviolios.java"
        implementationClass = "be.seeseemelk.raviolios.plugin.java.JavaPlugin"
    }
}
