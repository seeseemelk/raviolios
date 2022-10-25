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

group = "raviolios"
