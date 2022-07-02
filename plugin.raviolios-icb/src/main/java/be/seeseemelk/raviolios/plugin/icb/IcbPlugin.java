package be.seeseemelk.raviolios.plugin.icb;

import org.gradle.api.Plugin;
import org.gradle.api.Project;
import org.gradle.api.file.FileCollection;
import org.gradle.api.plugins.JavaPluginExtension;

import java.io.File;

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
		task.dependsOn("jar");

		project.afterEvaluate(it ->
		{
			FileCollection tree = project.files();

			for (File file : project.getConfigurations().getByName("runtimeClasspath").getFiles())
			{
				if (file.isDirectory())
					tree = tree.plus(project.fileTree(file));
				else
					tree = tree.plus(project.zipTree(file));
			}

			tree = tree.plus(project.getExtensions().getByType(JavaPluginExtension.class).getSourceSets().getByName("main").getOutput());

			task.from(tree);
			task.getOutput().set(new File(project.getBuildDir(), "icb/output.icb"));
		});
	}
}
