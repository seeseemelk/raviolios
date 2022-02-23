package be.seeseemelk.raviolios.icb;

import org.objectweb.asm.ClassReader;

import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.util.Collection;

public class IcbPacker implements AutoCloseable
{
	private final OutputStream output;

	public IcbPacker(Path outputFile) throws IOException
	{
		Files.createDirectories(outputFile.getParent());
		output = new BufferedOutputStream(Files.newOutputStream(outputFile, StandardOpenOption.CREATE, StandardOpenOption.TRUNCATE_EXISTING));

		output.write("ICB1".getBytes());
	}

	@Override
	public void close() throws IOException
	{
		output.close();
	}

	public void addFile(Path classFile) throws IOException
	{
		byte[] content = Files.readAllBytes(classFile);

		// Write tag
		output.write(Tag.CLASS.asByte());

		// Write null-terminated string
		writeZString(getClassNameOf(classFile));

		// Write the length of the class file
		writeU32(content.length);

		// Write class file
		writeBytes(content);
	}

	public void addFiles(Collection<Path> classFiles) throws IOException
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

	private void writeZString(String str) throws IOException
	{
		output.write(str.getBytes());
		output.write(0);
	}

	private void writeBytes(byte[] array) throws IOException
	{
		output.write(array);
	}

	private void writeU32(int num) throws IOException
	{
		writeU16(num);
		writeU16(num >> 16); //uwu
	}

	private void writeU16(int num) throws IOException
	{
		output.write(num);
		output.write(num >> 8);
	}
}
