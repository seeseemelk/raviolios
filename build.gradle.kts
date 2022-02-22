plugins {
    base
}

tasks.check {
    gradle.includedBuilds.forEach {
        dependsOn(it.task(":check"))
    }
}

tasks.clean {
    gradle.includedBuilds.forEach {
        dependsOn(it.task(":clean"))
    }
}
