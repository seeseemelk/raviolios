plugins {
    id("be.seeseemelk.raviolios.java")
    `java-gradle-plugin`
}

repositories {
    mavenCentral()
}

dependencies {
    implementation("raviolios:lib.icb")
}

gradlePlugin {
    val `raviolios-icb` by plugins.creating {
        id = "be.seeseemelk.raviolios.icb"
        implementationClass = "be.seeseemelk.raviolios.plugin.icb.IcbPlugin"
    }
}
