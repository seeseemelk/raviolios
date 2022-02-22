package be.seeseemelk.raviolios.plugin.icb;

import be.seeseemelk.raviolios.icb.IcbPacker;
import org.gradle.api.DefaultTask;
import org.gradle.api.file.FileCollection;
import org.gradle.api.provider.Property;
import org.gradle.api.tasks.InputFiles;
import org.gradle.api.tasks.OutputFile;
import org.gradle.api.tasks.TaskAction;

import java.io.File;
import java.io.IOException;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;

public abstract class IcbTask extends DefaultTask
{
	@InputFiles
	public abstract Property<FileCollection> getClasses();

	@OutputFile
	public abstract Property<File> getOutput();

	private void addClassFiles(Collection<File> classFiles, File directory)
	{
		classFiles.addAll(Arrays.asList(directory.listFiles((dir, name) -> name.endsWith(".class"))));
		Arrays.stream(directory.listFiles(File::isDirectory)).forEach(dir -> addClassFiles(classFiles, dir));
	}

	@TaskAction
	public void bundleClasses()
	{
		try
		{
			Collection<File> classes = new ArrayList<>();
			getClasses().get().getFiles().forEach(dir -> addClassFiles(classes, dir));

			try (IcbPacker packer = new IcbPacker(getOutput().get().toPath()))
			{
				for (File file : classes)
				{
					if (file.getName().equals("module-info.class"))
						continue;
					Path toPath = file.toPath();
					packer.addFile(toPath);
				}
			}
		}
		catch (IOException exception)
		{
			throw new RuntimeException("Could not create ICB bundle", exception);
		}
	}
}
