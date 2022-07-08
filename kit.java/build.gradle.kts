plugins {
    id("be.seeseemelk.raviolios.java")
    id("io.freefair.lombok") version "6.4.1"
    `java-library`
}

repositories {
    mavenCentral()
}

group = "raviolios"

dependencies {
    testImplementation("org.junit.jupiter:junit-jupiter:5.8.1")
}

tasks.named<Test>("test") {
    useJUnitPlatform()
}
