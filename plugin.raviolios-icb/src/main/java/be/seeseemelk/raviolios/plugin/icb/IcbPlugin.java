package be.seeseemelk.raviolios.plugin.icb;

import org.gradle.api.Action;
import org.gradle.api.Project;
import org.gradle.api.Plugin;
import org.gradle.api.Task;
import org.gradle.api.artifacts.Dependency;
import org.gradle.api.initialization.IncludedBuild;
import org.gradle.api.plugins.JavaPluginExtension;
import org.gradle.api.tasks.compile.JavaCompile;
import org.gradle.jvm.toolchain.JavaLanguageVersion;

import java.io.File;

/**
 * Builds ICB files
 */
public class IcbPlugin implements Plugin<Project>
{
	public void apply(Project project)
	{
		IcbTask task = project.getTasks().create("icb", IcbTask.class);

		project.afterEvaluate(it -> {
			project.getConfigurations().getByName("java");
			/*for (Dependency dependency : project.getDependencies())
			{

			}
			project.getDependencies().getExtensions().findByName("java").
			task.getClasses()*/
		});
	}
}
