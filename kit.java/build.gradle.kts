plugins {
    id("be.seeseemelk.raviolios.java")
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
