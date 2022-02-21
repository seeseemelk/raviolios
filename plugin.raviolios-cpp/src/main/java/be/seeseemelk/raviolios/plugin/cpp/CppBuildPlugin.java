/*
 * This Java source file was generated by the Gradle 'init' task.
 */
package be.seeseemelk.raviolios.plugin.cpp;

import org.gradle.api.Action;
import org.gradle.api.Project;
import org.gradle.api.Plugin;
import org.gradle.api.Task;
import org.gradle.api.initialization.IncludedBuild;

import java.io.File;

/**
 * Allows building C/C++ kernel sources.
 */
public class CppBuildPlugin implements Plugin<Project>
{
	public void apply(Project project)
	{
		project.getPlugins().apply("base");
		project.getConfigurations().create("source");
		CppExtension extensions = project.getExtensions().create("cpp", CppExtension.class);
		CppBuildTask task = project.getTasks().create("compileCpp", CppBuildTask.class);
		task.getOutputDirectory().convention(project.getLayout().getBuildDirectory().dir("cpp"));
		task.setDescription("Builds C/C++ sources");

		project.getTasks().getByName("assemble").dependsOn(task);

		project.afterEvaluate(proj ->
		{
			for (String dependency : extensions.getDependencies().get())
			{
				IncludedBuild build = project.getGradle().includedBuild(dependency);
				File include = new File(build.getProjectDir(), "src/main/include");
				File cpp = new File(build.getProjectDir(), "src/main/cpp");
				task.addIncludes(project.fileTree(include));
				task.addCppSources(project.fileTree(cpp));
				task.getProjects().add(dependency);
				task.getSourceRoots().add(cpp);
			}
			task.addIncludes(project.fileTree("src/main/include"));
			task.addCppSources(project.fileTree("src/main/cpp"));
			task.getCommonFlags().addAll(extensions.getCommonFlags().get());
			task.getCppFlags().addAll(extensions.getCppFlags().get());
			task.getLdFlags().addAll(extensions.getLdFlags().get());
			task.getAsFlags().addAll(extensions.getAsFlags().get());
			task.getProjects().add(project.getName());
			task.getSourceRoots().add(project.file("src/main/cpp"));
		});

	}
}
