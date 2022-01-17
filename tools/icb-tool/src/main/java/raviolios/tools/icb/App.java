package raviolios.tools.icb;

import java.io.IOException;
import java.nio.file.Path;
import java.util.Arrays;
import java.util.Set;
import java.util.stream.Collectors;

public class App
{
	private static void printHelp()
	{
		System.out.println("Usage: output input...");
	}

	public static void main(String[] args) throws IOException
	{
		System.out.format("Current working directory: %s%n", System.getProperty("user.dir"));
		if (args.length < 2)
		{
			printHelp();
			System.exit(1);
		}
		else
		{
			Path outputFile = Path.of(args[0]);
			try (IcbPacker packer = new IcbPacker(outputFile))
			{
				Set<Path> inputs = Arrays.stream(args)
						.skip(1)
						.map(Path::of)
						.collect(Collectors.toSet());
				packer.addFiles(inputs);
			}
		}
	}
}
