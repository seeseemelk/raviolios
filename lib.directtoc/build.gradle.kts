plugins {
    id("be.seeseemelk.raviolios.java")
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
