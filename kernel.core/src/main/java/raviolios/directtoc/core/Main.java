package raviolios.directtoc.core;

import be.seeseemelk.directtoc.ProgramWriter;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;

public class Main
{
	public static void main(String[] args) throws IOException
	{
		Path destination = Path.of(args[0]);
		System.out.println("Will be generating sources into: " + destination);

		ProgramWriter writer = new ProgramWriter("core");
		writer.addFunction(Utils.stringLength);

		Files.createDirectories(destination);
		Files.writeString(destination.resolve("d2c.c"), writer.toC().toOutput());
		Files.writeString(destination.resolve("d2c.h"), writer.toH().toOutput());
	}
}
