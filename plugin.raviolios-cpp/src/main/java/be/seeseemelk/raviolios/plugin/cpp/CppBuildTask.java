package be.seeseemelk.raviolios.plugin.cpp;

import lombok.Getter;
import lombok.Setter;
import org.gradle.api.DefaultTask;
import org.gradle.api.file.ConfigurableFileCollection;
import org.gradle.api.file.DirectoryProperty;
import org.gradle.api.file.FileCollection;
import org.gradle.api.tasks.Input;
import org.gradle.api.tasks.InputFiles;
import org.gradle.api.tasks.OutputDirectory;
import org.gradle.api.tasks.TaskAction;
import org.gradle.workers.WorkQueue;
import org.gradle.workers.WorkerExecutor;

import javax.inject.Inject;
import java.io.File;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public abstract class CppBuildTask extends DefaultTask
{
	@InputFiles
	public abstract ConfigurableFileCollection getIncludes();

	public void addIncludes(FileCollection collection)
	{
		getIncludes().setFrom(getIncludes().plus(collection));
	}

	@InputFiles
	public abstract ConfigurableFileCollection getCppSources();

	public void addCppSources(FileCollection collection)
	{
		getCppSources().setFrom(getCppSources().plus(collection));
	}

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
	private final List<String> projects = new ArrayList<>();

	@Getter
	@Input
	private final List<File> sourceRoots = new ArrayList<>();

	@OutputDirectory
	public abstract DirectoryProperty getOutputDirectory();

	@Inject
	public abstract WorkerExecutor getWorkExecutor();

	@TaskAction
	public void build()
	{
		WorkQueue queue = getWorkExecutor().noIsolation();
		List<File> objects = new ArrayList<>();
		buildCpp(queue, objects);
		queue.await();
		link(objects);
	}

	private Set<File> getIncludeDirectories()
	{
		Set<File> dirs = new HashSet<>();
		for (File file : getIncludes().getFiles())
			dirs.add(file.getParentFile());
		return dirs;
	}

	private void buildCpp(WorkQueue queue, List<File> objects)
	{
		List<String> command = new ArrayList<>();
		command.add("clang++");

		command.addAll(getCommonFlags());
		command.addAll(getCppFlags());

		for (File includeDirectory : getIncludeDirectories())
			command.add("-I" + includeDirectory.getAbsolutePath());
		command.add("-c");

		for (File source : getCppSources().getFiles())
		{
			String projectName = getProjectNameOf(source);
			String sourceRoot = getPathRelativeToSourceRoot(source);

			File outputDirectory = new File(getOutputDirectory().getAsFile().get(), projectName);
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
		for (File sourceRoot : sourceRoots)
		{
			Path rootPath = sourceRoot.toPath();
			if (sourcePath.startsWith(rootPath))
				return rootPath.relativize(sourcePath).toString();
		}
		throw new RuntimeException("Could not determine source root for " + source.getAbsolutePath());
	}

	private String getProjectNameOf(File source)
	{
		String path = source.getAbsolutePath();
		for (String dependency : projects)
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
