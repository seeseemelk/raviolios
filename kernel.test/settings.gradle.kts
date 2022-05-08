rootProject.name = "kernel.test"

pluginManagement {
    includeBuild("../plugin.raviolios-cpp")
    includeBuild("../plugin.raviolios-java")
}

includeBuild("../kernel.core")
includeBuild("../kit.java")
