plugins {
    id("be.seeseemelk.raviolios.java")
    `java-library`
}

group = "raviolios"

repositories {
    mavenCentral()
}

dependencies {
    implementation("org.ow2.asm:asm-util:9.2")
    testImplementation("org.junit.jupiter:junit-jupiter:5.8.2")
}

tasks.named<Test>("test") {
    useJUnitPlatform()
}
