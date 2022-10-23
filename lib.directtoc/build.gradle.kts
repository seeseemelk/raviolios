plugins {
    id("be.seeseemelk.raviolios.java")
    id("io.freefair.lombok") version "6.5.1"
    `java-library`
}

group = "raviolios"

repositories {
    mavenCentral()
}

dependencies {
    testImplementation("org.hamcrest:hamcrest:2.2")
    testImplementation("org.junit.jupiter:junit-jupiter-api:5.9.0")
    testRuntimeOnly("org.junit.jupiter:junit-jupiter-engine:5.9.0")
}

tasks.named<Test>("test") {
    useJUnitPlatform()
}
