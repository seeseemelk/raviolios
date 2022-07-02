/*
 * This Java source file was generated by the Gradle 'init' task.
 */
package be.seeseemelk.raviolios.plugin.java;

import org.gradle.api.Plugin;
import org.gradle.api.Project;
import org.gradle.api.Task;
import org.gradle.api.initialization.IncludedBuild;
import org.gradle.api.plugins.JavaPluginExtension;
import org.gradle.jvm.toolchain.JavaLanguageVersion;

/**
 * Sets default configuration for Raviolios Java projects
 */
public class JavaPlugin implements Plugin<Project>
{
	public void apply(Project project)
	{
		project.getPlugins().apply("java");

		JavaPluginExtension extension = project.getExtensions().getByType(JavaPluginExtension.class);
		extension.getToolchain().getLanguageVersion().convention(JavaLanguageVersion.of(11));

		Task task = project.getTasks().getByName("jar");
		for (IncludedBuild includedBuild : project.getGradle().getIncludedBuilds())
		{
			task.dependsOn(includedBuild.task(":jar"));
		}
	}
}
