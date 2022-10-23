package be.seeseemelk.raviolios.plugin.cpp;

import be.seeseemelk.raviolios.plugin.cli.Arguments;
import be.seeseemelk.raviolios.plugin.cli.CommandRunner;
import be.seeseemelk.raviolios.plugin.cli.docker.DockerRunner;
import be.seeseemelk.raviolios.plugin.cli.local.LocalRunner;
import be.seeseemelk.raviolios.plugin.cpp.makefile.MakefileDependencies;
import be.seeseemelk.raviolios.plugin.cpp.makefile.MakefileParser;
import be.seeseemelk.raviolios.plugin.cpp.makefile.RebuildTracker;
import lombok.Getter;
import org.gradle.api.DefaultTask;
import org.gradle.api.file.ConfigurableFileCollection;
import org.gradle.api.file.DirectoryProperty;
import org.gradle.api.file.FileCollection;
import org.gradle.api.file.RegularFileProperty;
import org.gradle.api.provider.Property;
import org.gradle.api.tasks.*;
import org.gradle.work.FileChange;
import org.gradle.work.Incremental;
import org.gradle.work.InputChanges;
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
	public abstract ConfigurableFileCollection getCSources();

	public void addCSources(FileCollection collection)
	{
		getCSources().setFrom(getCSources().plus(collection));
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

	@Input
	public abstract Property<Boolean> getUseDocker();

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

		try (CommandRunner runner = createRunner())
		{
			List<File> objects = new ArrayList<>();
			buildCpp(runner, objects, tracker);
			buildAssembler(runner, objects, tracker);
			runner.await();
			link(runner, objects);
		}
		catch (Exception exception)
		{
			throw new CppBuildException("Could not build native object", exception);
		}
	}

	private CommandRunner createRunner()
	{
		if (getUseDocker().get())
			return createDockerRunner();
		else
			return createLocalRunner();
	}

	private CommandRunner createDockerRunner()
	{
		DockerRunner runner = new DockerRunner();
		runner.start();
		return runner;
	}

	private CommandRunner createLocalRunner()
	{
		LocalRunner runner = new LocalRunner(getWorkExecutor());
		return runner;
	}

	private MakefileDependencies parseMakefiles()
	{
		try
		{
			MakefileParser parser = new MakefileParser();
			FileCollection dependencies = getOutputDirectory()
				.getAsFileTree()
				.filter(file -> file.getName().endsWith(".d"));
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

	private void addIncludes(Arguments command)
	{
		for (File includeDirectory : getIncludeDirectories())
			command.add("-I" + includeDirectory.getAbsolutePath());
	}

	private void buildC(
		CommandRunner runner,
		List<File> objects,
		RebuildTracker tracker
	)
	{
		buildClang(runner, objects, tracker, "clang", getCSources());
	}

	private void buildCpp(
		CommandRunner runner,
		List<File> objects,
		RebuildTracker tracker
	)
	{
		buildClang(runner, objects, tracker, "clang++", getCppSources());
	}

	private void buildClang(
		CommandRunner runner,
		List<File> objects,
		RebuildTracker tracker,
		String compiler,
		ConfigurableFileCollection sources
	)
	{
		Arguments command = new Arguments();
		command.add(compiler);

		command.addAll(getCommonFlags());
		command.addAll(getCppFlags());
		addIncludes(command);
		runBuild(runner, command, objects, tracker, sources);
	}

	private void buildAssembler(
		CommandRunner runner,
		List<File> objects,
		RebuildTracker tracker
	)
	{
		Arguments command = new Arguments();
		command.add("clang");
		command.addAll(getCommonFlags());
		command.addAll(getAsFlags());
		addIncludes(command);
		runBuild(runner, command, objects, tracker, getAssemblerSources());
	}

	private void runBuild(
		CommandRunner runner,
		Arguments baseCommand,
		List<File> objects,
		RebuildTracker tracker,
		ConfigurableFileCollection sources
	)
	{
		baseCommand.add("-c").add("-MP").add("-MD");

		for (File source : sources.getFiles())
		{
			Arguments command = new Arguments(baseCommand);
			String projectName = getProjectNameOf(source);
			String sourceRoot = getPathRelativeToSourceRoot(source);

			File outputDirectory = new File(getOutputDirectory().getAsFile().get(), projectName);
			outputDirectory = new File(outputDirectory, sourceRoot);
			File object = new File(outputDirectory, source.getName() + ".o");
			objects.add(object);

			if (tracker.needsRebuild(source.getAbsolutePath()))
			{
				command.add("-o").addOutput(object).addSource(source);
				runner.run(command);
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

	private void link(CommandRunner runner, List<File> objects)
	{
		Arguments command = new Arguments();
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
		command.addOutput(outputObject);

		for (File object : objects)
		{
			command.addSource(object);
		}

		getLogger().debug("Executing " + command);
		runner.run(command);
	}
}
