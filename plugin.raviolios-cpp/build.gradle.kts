plugins {
    `java-gradle-plugin`
    id("io.freefair.lombok") version "6.4.1"
}

repositories {
    mavenCentral()
}

dependencies {
    implementation("com.github.docker-java:docker-java-core:3.2.13")
    implementation("com.github.docker-java:docker-java-transport-httpclient5:3.2.13")

    testImplementation("org.junit.jupiter:junit-jupiter:5.8.1")
}

gradlePlugin {
    val `raviolios-cpp-source` by plugins.creating {
        id = "be.seeseemelk.raviolios.cppsource"
        implementationClass = "be.seeseemelk.raviolios.plugin.cpp.CppSourcePlugin"
    }
    val `raviolios-cpp` by plugins.creating {
        id = "be.seeseemelk.raviolios.cpp"
        implementationClass = "be.seeseemelk.raviolios.plugin.cpp.CppBuildPlugin"
    }
}

tasks.named<Test>("test") {
    useJUnitPlatform()
}
