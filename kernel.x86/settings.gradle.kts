rootProject.name = "kernel.x86"

pluginManagement {
    includeBuild("../plugin.raviolios-cpp")
    includeBuild("../plugin.raviolios-java")
    includeBuild("../plugin.raviolios-icb")
}

includeBuild("../kernel.core")
