package be.seeseemelk.raviolios.plugin.icb;

import be.seeseemelk.raviolios.icb.IcbPacker;
import org.gradle.api.file.DuplicatesStrategy;
import org.gradle.api.internal.file.copy.CopyAction;
import org.gradle.api.provider.Property;
import org.gradle.api.tasks.AbstractCopyTask;
import org.gradle.api.tasks.OutputFile;
import org.gradle.api.tasks.WorkResults;

import javax.inject.Inject;
import java.io.File;
import java.io.IOException;
import java.nio.file.Path;
import java.util.Objects;
import java.util.Set;

public abstract class IcbTask extends AbstractCopyTask
{
	@OutputFile
	public abstract Property<File> getOutput();

	@Inject
	public IcbTask()
	{
		super();
		setDuplicatesStrategy(DuplicatesStrategy.INCLUDE);
	}

	@Override
	protected CopyAction createCopyAction()
	{
		return stream ->
		{
			try (IcbPacker packer = new IcbPacker(getOutput().get().toPath()))
			{
				stream.process(details ->
				{
					try
					{
						addToBundle(packer, details.getFile());
					}
					catch (IOException e)
					{
						throw new RuntimeException(e);
					}
				});
			}
			catch (IOException exception)
			{
				throw new RuntimeException("Could not create ICB bundle", exception);
			}
			return WorkResults.didWork(true);
		};
	}

	private void addToBundle(IcbPacker packer, Set<File> files) throws IOException
	{
		for (File file : files)
		{
			addToBundle(packer, file);
		}
	}

	private void addToBundle(IcbPacker packer, File file) throws IOException
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
