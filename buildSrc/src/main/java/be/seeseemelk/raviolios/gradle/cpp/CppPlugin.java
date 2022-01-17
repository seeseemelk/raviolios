package be.seeseemelk.raviolios.gradle.cpp;

import org.gradle.api.Plugin;
import org.gradle.api.Project;
import org.gradle.api.Task;
import org.gradle.api.plugins.JavaPluginExtension;
import org.gradle.api.tasks.SourceSet;
import org.gradle.api.tasks.SourceSetContainer;

@SuppressWarnings("unused")
public class CppPlugin implements Plugin<Project>
{
	@Override
	public void apply(Project project)
	{
		CppExtension extension = project.getExtensions().create("cpp", CppExtension.class);

		//JavaPluginExtension extension = project.getExtensions().getByType(JavaPluginExtension.class);

		Task task = project.task("cpp");
		task.doLast(t -> {
			System.out.println("Prefix: " + extension.getPrefix());
		});
		task.setGroup("raviolios");
		task.setDescription("Builds C/C++ sources");
	}
}
