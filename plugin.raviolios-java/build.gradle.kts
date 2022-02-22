plugins {
    `java-gradle-plugin`
}

repositories {
    mavenCentral()
}

dependencies {
    //testImplementation("org.junit.jupiter:junit-jupiter:5.8.1")
    //testImplementation("org.hamcrest:hamcrest-all:1.3")
}

gradlePlugin {
    val `raviolios-java` by plugins.creating {
        id = "be.seeseemelk.raviolios.java"
        implementationClass = "be.seeseemelk.raviolios.plugin.java.JavaPlugin"
    }
}

//tasks.named<Test>("test") {
//    // Use JUnit Jupiter for unit tests.
//    useJUnitPlatform()
//}
