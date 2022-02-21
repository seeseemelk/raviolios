package be.seeseemelk.raviolios.plugin.cpp;

import lombok.Getter;
import org.gradle.api.DefaultTask;
import org.gradle.api.file.DirectoryProperty;
import org.gradle.api.file.DirectoryTree;
import org.gradle.api.file.SourceDirectorySet;
import org.gradle.api.invocation.Gradle;
import org.gradle.api.model.ObjectFactory;
import org.gradle.api.tasks.*;
import org.gradle.workers.WorkQueue;
import org.gradle.workers.WorkerExecutor;

import javax.inject.Inject;
import java.io.File;
import java.io.IOException;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;

public abstract class CppBuildTask extends DefaultTask
{
	@Getter
	@InputFiles
	private final SourceDirectorySet include;

	@Getter
	@InputFiles
	private final SourceDirectorySet cpp;

	@Getter
	@Input
	private final List<String> commonFlags = new ArrayList<>();

	@Getter
	@Input
	private final List<String> cppFlags = new ArrayList<>();

	@Getter
	@Input
	private final List<String> ldFlags = new ArrayList<>();

	@Getter
	@Input
	private final List<String> asFlags = new ArrayList<>();

	@Getter
	@Input
	private final List<String> dependencies = new ArrayList<>();

	@OutputDirectory
	public abstract DirectoryProperty getOutputDirectory();

	@Inject
	public abstract WorkerExecutor getWorkExecutor();

	@Inject
	public abstract Gradle getGradle();

	@Inject
	public CppBuildTask(ObjectFactory objectFactory)
	{
		include = objectFactory.sourceDirectorySet("include", "C/C++ includes");
		cpp = objectFactory.sourceDirectorySet("cpp", "C/C++ sources");
	}

	@TaskAction
	public void build()
	{
		WorkQueue queue = getWorkExecutor().noIsolation();
		List<File> objects = new ArrayList<>();
		buildCpp(queue, objects);
		queue.await();
		link(objects);
	}

	private void buildCpp(WorkQueue queue, List<File> objects)
	{
		List<String> command = new ArrayList<>();
		command.add("clang++");

		command.addAll(getCommonFlags());
		command.addAll(getCppFlags());

		for (File includeDirectory : include.getSrcDirs())
			command.add("-I" + includeDirectory.getAbsolutePath());
		command.add("-c");

		for (File source : cpp.getFiles())
		{
			String dependency = getDependencyFor(source);
			String sourceRoot = getPathRelativeToSourceRoot(source);

			File outputDirectory = new File(getOutputDirectory().getAsFile().get(), dependency);
			outputDirectory = new File(outputDirectory, sourceRoot);
			File object = new File(outputDirectory, source.getName() + ".o");
			objects.add(object);

			queue.submit(CppBuildWorkAction.class, parameters -> {
				parameters.getCommand().value(command);
				parameters.getOutput().fileValue(object);
				parameters.getSource().fileValue(source);
			});
		}
	}

	private String getPathRelativeToSourceRoot(File source)
	{
		Path sourcePath = source.toPath();
		for (File sourceRoot : cpp.getSrcDirs())
		{
			Path rootPath = sourceRoot.toPath();
			if (sourcePath.startsWith(rootPath))
				return rootPath.relativize(sourcePath).toString();
		}
		throw new RuntimeException("Could not determine source root for " + source.getAbsolutePath());
	}

	private String getDependencyFor(File source)
	{
		String path = source.getAbsolutePath();
		for (String dependency : dependencies)
		{
			if (path.contains(dependency))
				return dependency;
		}
		throw new RuntimeException("Could not find project corresponding to " + path);
	}

	private void link(List<File> objects)
	{
		List<String> command = new ArrayList<>();
		command.add("clang++");

		command.addAll(getCommonFlags());
		command.addAll(getLdFlags());

		File outputDirectory = getOutputDirectory().getAsFile().get();
		File outputObject = new File(outputDirectory, "output.elf");
		command.add("-o");
		command.add(outputObject.getAbsolutePath());

		for (File object : objects)
		{
			command.add(object.getAbsolutePath());
		}

		CppBuildWorkAction.runCommand(command);
	}
}
