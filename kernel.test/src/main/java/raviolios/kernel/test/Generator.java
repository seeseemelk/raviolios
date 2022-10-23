package raviolios.kernel.test;

import be.seeseemelk.directtoc.ProgramWriter;
import be.seeseemelk.directtoc.types.Function;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

import static be.seeseemelk.directtoc.CommonSyntax.Func;
import static be.seeseemelk.directtoc.CommonTypes.VOID;

public class Generator
{
	public static void main(String[] args) throws IOException
	{
		String filename = "d2c_raviolios";
		Path fileC = Paths.get(args[0], filename + ".c");
		Path fileH = Paths.get(args[1], filename + ".h");

		Files.createDirectories(Paths.get(args[0]));
		Files.createDirectories(Paths.get(args[1]));

		Function main = Func(VOID, "Test");

		ProgramWriter writer = new ProgramWriter(filename);
		writer.addFunction(main);

		Files.writeString(fileC, writer.toC().toOutput());
		Files.writeString(fileH, writer.toH().toOutput());
	}
}
