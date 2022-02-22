package be.seeseemelk.raviolios.plugin.cpp.makefile;

import lombok.AllArgsConstructor;
import lombok.Getter;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.Reader;

@AllArgsConstructor
public class MakefileParser
{
	@Getter
	private final MakefileDependencies dependencies;

	public MakefileParser()
	{
		this(new MakefileDependencies());
	}

	public void parse(BufferedReader reader) throws IOException
	{
		String target = null;
		String line;
		while ((line = reader.readLine()) != null)
		{
			line = line.replace("\\", "");
			if (line.contains(":"))
			{
				int split = line.indexOf(':');
				target = line.substring(0, split).trim();
				line = line.substring(split + 1);
			}

			line = line.trim();
			if (!line.isEmpty())
			{
				if (target == null)
					throw new NullPointerException("Target was not set before a dependency was found");
				dependencies.addDependency(target, line);
			}
		}
	}
}
