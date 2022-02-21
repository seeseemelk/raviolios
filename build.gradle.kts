plugins {
    base
}

tasks.check {
    dependsOn(gradle.includedBuild("kernel.test").task(":check"))
}

tasks.clean {
    dependsOn(gradle.includedBuild("kernel.test").task(":clean"))
}
