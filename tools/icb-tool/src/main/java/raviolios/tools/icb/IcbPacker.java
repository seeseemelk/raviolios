package raviolios.tools.icb;

import org.objectweb.asm.ClassReader;
import org.objectweb.asm.tree.ClassNode;

import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.util.Set;

public class IcbPacker implements AutoCloseable
{
	private final OutputStream output;

	public IcbPacker(Path outputFile) throws IOException
	{
		Files.createDirectories(outputFile.getParent());
		output = new BufferedOutputStream(Files.newOutputStream(outputFile, StandardOpenOption.CREATE));

		output.write("ICB".getBytes());
	}

	@Override
	public void close() throws IOException
	{
		output.close();
	}

	public void addFile(Path classFile) throws IOException
	{
		// Write tag
		output.write(Tag.CLASS.asByte());

		// Write null-terminated string
		String name = getClassNameOf(classFile);
		output.write(name.getBytes());
		output.write(0);

		// Write class file
		output.write(Files.readAllBytes(classFile));
	}

	public void addFiles(Set<Path> classFiles) throws IOException
	{
		for (Path classFile : classFiles)
		{
			addFile(classFile);
		}
	}

	private String getClassNameOf(Path classFile) throws IOException
	{
		ClassReader reader = new ClassReader(Files.newInputStream(classFile, StandardOpenOption.READ));
		return reader.getClassName();
	}
}
