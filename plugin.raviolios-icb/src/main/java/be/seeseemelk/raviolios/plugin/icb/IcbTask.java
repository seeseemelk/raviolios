package be.seeseemelk.raviolios.plugin.icb;

import be.seeseemelk.raviolios.icb.IcbPacker;
import org.gradle.api.DefaultTask;
import org.gradle.api.file.ConfigurableFileCollection;
import org.gradle.api.provider.Property;
import org.gradle.api.tasks.InputFiles;
import org.gradle.api.tasks.OutputFile;
import org.gradle.api.tasks.TaskAction;

import java.io.File;
import java.io.IOException;
import java.nio.file.Path;
import java.util.Objects;
import java.util.Set;

public abstract class IcbTask extends DefaultTask
{
	@InputFiles
	public abstract ConfigurableFileCollection getClasses();

	@OutputFile
	public abstract Property<File> getOutput();

	@TaskAction
	public void bundleClasses()
	{
		try
		{
			try (IcbPacker packer = new IcbPacker(getOutput().get().toPath()))
			{
				addToBundle(packer, getClasses().getFiles());
			}
		}
		catch (IOException exception)
		{
			throw new RuntimeException("Could not create ICB bundle", exception);
		}
	}

	private void addToBundle(IcbPacker packer, Set<File> files) throws IOException
	{
		for (File file : files)
		{
			if (file.isDirectory())
			{
				addToBundle(packer, Set.of(Objects.requireNonNull(file.listFiles())));
			}
			else if (file.getName().endsWith(".class") && !file.getName().equals("module-info.class"))
			{
				Path fileToAdd = file.toPath();
				getLogger().debug("Adding " + fileToAdd);
				packer.addFile(fileToAdd);
			}
		}
	}
}
