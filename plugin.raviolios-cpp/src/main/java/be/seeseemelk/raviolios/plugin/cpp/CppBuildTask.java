package be.seeseemelk.raviolios.plugin.cpp;

import be.seeseemelk.raviolios.plugin.cpp.makefile.MakefileDependencies;
import be.seeseemelk.raviolios.plugin.cpp.makefile.MakefileParser;
import be.seeseemelk.raviolios.plugin.cpp.makefile.RebuildTracker;
import lombok.Getter;
import org.gradle.api.DefaultTask;
import org.gradle.api.file.ConfigurableFileCollection;
import org.gradle.api.file.DirectoryProperty;
import org.gradle.api.file.FileCollection;
import org.gradle.api.file.RegularFileProperty;
import org.gradle.api.tasks.*;
import org.gradle.work.FileChange;
import org.gradle.work.Incremental;
import org.gradle.work.InputChanges;
import org.gradle.workers.WorkQueue;
import org.gradle.workers.WorkerExecutor;

import javax.inject.Inject;
import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.function.Predicate;

@CacheableTask
public abstract class CppBuildTask extends DefaultTask
{
	@InputFiles
	@Incremental
	@PathSensitive(PathSensitivity.ABSOLUTE)
	@IgnoreEmptyDirectories
	public abstract ConfigurableFileCollection getIncludes();

	public void addIncludes(FileCollection collection)
	{
		getIncludes().setFrom(getIncludes().plus(collection));
	}

	@InputFiles
	@Incremental
	@PathSensitive(PathSensitivity.ABSOLUTE)
	@IgnoreEmptyDirectories
	public abstract ConfigurableFileCollection getCppSources();

	public void addCppSources(FileCollection collection)
	{
		getCppSources().setFrom(getCppSources().plus(collection));
	}

	@InputFiles
	@Incremental
	@PathSensitive(PathSensitivity.ABSOLUTE)
	@IgnoreEmptyDirectories
	public abstract ConfigurableFileCollection getAssemblerSources();

	public void addAssemblerSources(FileCollection collection)
	{
		getAssemblerSources().setFrom(getAssemblerSources().plus(collection));
	}

	@InputFile
	@Optional
	@PathSensitive(PathSensitivity.ABSOLUTE)
	public abstract RegularFileProperty getLinkerScript();

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
	public void build(InputChanges changes)
	{
		MakefileDependencies dependencies = parseMakefiles();
		RebuildTracker tracker = new RebuildTracker(dependencies);

		for (FileChange change : changes.getFileChanges(getIncludes()))
			tracker.markAsChanged(change.getFile().getAbsolutePath());
		for (FileChange change : changes.getFileChanges(getCppSources()))
			tracker.markAsChanged(change.getFile().getAbsolutePath());
		for (FileChange change : changes.getFileChanges(getAssemblerSources()))
			tracker.markAsChanged(change.getFile().getAbsolutePath());

		WorkQueue queue = getWorkExecutor().noIsolation();
		List<File> objects = new ArrayList<>();
		buildCpp(queue, objects, tracker);
		buildAssembler(queue, objects, tracker);
		queue.await();
		link(objects);
	}

	private MakefileDependencies parseMakefiles()
	{
		try
		{
			MakefileParser parser = new MakefileParser();
			FileCollection dependencies = getOutputDirectory().getAsFileTree().filter(file -> file.getName().endsWith(
				".d"));
			for (File dependency : dependencies)
			{
				getLogger().debug("Parsing " + dependency);
				parser.parse(Files.newBufferedReader(dependency.toPath()));
			}
			return parser.getDependencies();
		}
		catch (IOException exception)
		{
			throw new RuntimeException("Could not parse makefile", exception);
		}
	}

	private Set<File> getIncludeDirectories()
	{
		Set<File> dirs = new HashSet<>();
		for (File file : getIncludes().getFiles())
			dirs.add(file.getParentFile());
		return dirs;
	}

	private void buildCpp(WorkQueue queue, List<File> objects, RebuildTracker tracker)
	{
		List<String> command = new ArrayList<>();
		command.add("clang++");

		command.addAll(getCommonFlags());
		command.addAll(getCppFlags());

		for (File includeDirectory : getIncludeDirectories())
			command.add("-I" + includeDirectory.getAbsolutePath());
		runBuild(command, queue, objects, tracker, getCppSources());
	}

	private void buildAssembler(WorkQueue queue, List<File> objects, RebuildTracker tracker)
	{
		List<String> command = new ArrayList<>();
		command.add("clang");
		command.addAll(getCommonFlags());
		command.addAll(getAsFlags());
		runBuild(command, queue, objects, tracker, getAssemblerSources());
	}

	private void runBuild(
		List<String> command,
		WorkQueue queue,
		List<File> objects,
		RebuildTracker tracker,
		ConfigurableFileCollection sources
	)
	{
		command.add("-c");

		command.add("-MP");
		command.add("-MD");

		for (File source : sources.getFiles())
		{
			String projectName = getProjectNameOf(source);
			String sourceRoot = getPathRelativeToSourceRoot(source);

			File outputDirectory = new File(getOutputDirectory().getAsFile().get(), projectName);
			outputDirectory = new File(outputDirectory, sourceRoot);
			File object = new File(outputDirectory, source.getName() + ".o");
			objects.add(object);

			if (tracker.needsRebuild(source.getAbsolutePath()))
			{
				queue.submit(CppBuildWorkAction.class, parameters ->
				{
					parameters.getCommand().value(command);
					parameters.getOutput().fileValue(object);
					parameters.getSource().fileValue(source);
				});
			}
		}
	}

	private String getPathRelativeToSourceRoot(File source)
	{
		Path sourcePath = source.toPath().getParent();
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

		if (getLinkerScript().isPresent())
		{
			command.add("-T");
			command.add(getLinkerScript().getAsFile().get().getAbsolutePath());
		}

		File outputDirectory = getOutputDirectory().getAsFile().get();
		File outputObject = new File(outputDirectory, "output.elf");
		command.add("-o");
		command.add(outputObject.getAbsolutePath());

		for (File object : objects)
		{
			command.add(object.getAbsolutePath());
		}

		getLogger().debug("Executing " + String.join(" ", command));
		CppBuildWorkAction.runCommand(command);
	}
}
