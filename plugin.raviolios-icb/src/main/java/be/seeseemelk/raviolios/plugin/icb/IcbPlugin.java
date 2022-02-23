package be.seeseemelk.raviolios.plugin.icb;

import org.gradle.api.Plugin;
import org.gradle.api.Project;
import org.gradle.api.plugins.JavaPluginExtension;

import java.io.File;
import java.util.HashSet;
import java.util.Set;

/**
 * Builds ICB files
 */
public class IcbPlugin implements Plugin<Project>
{
	public void apply(Project project)
	{
		IcbTask task = project.getTasks().create("icb", IcbTask.class);
		task.setGroup("build");
		task.setDescription("Generates an ICB file");
		task.dependsOn("classes");

		Set<File> files = new HashSet<>();

		project.getTasks().getByName("classes").doLast(unused -> {
			project.getConfigurations().getByName("runtimeClasspath").forEach(file -> {
				if (file.isDirectory())
				{
					files.addAll(project.fileTree(file).getFiles());
				}
				else
				{
					files.addAll(project.zipTree(file).getFiles());
				}
			});

			files.addAll(project.getExtensions().getByType(JavaPluginExtension.class).getSourceSets().getByName("main").getOutput().getFiles());
			task.getClasses().setFrom(files);
		});

		project.afterEvaluate(it -> task.getOutput().set(new File(project.getBuildDir(), "icb/output.icb")));
	}
}
