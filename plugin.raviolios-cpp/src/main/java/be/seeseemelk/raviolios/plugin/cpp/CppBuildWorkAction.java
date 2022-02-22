package be.seeseemelk.raviolios.plugin.cpp;

import org.codehaus.groovy.util.StringUtil;
import org.gradle.workers.WorkAction;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public abstract class CppBuildWorkAction implements WorkAction<CppBuildWorkParameters>
{
	@Override
	public void execute()
	{
		File source = getParameters().getSource().getAsFile().get();
		File destination = getParameters().getOutput().getAsFile().get();

		destination.getParentFile().mkdirs();

		List<String> command = new ArrayList<>(getParameters().getCommand().get());
		command.add("-o");
		command.add(destination.getAbsolutePath());
		command.add(source.getAbsolutePath());

		runCommand(command);
	}

	public static void runCommand(List<String> command)
	{
		try
		{

			ProcessBuilder builder = new ProcessBuilder(command);
			Process process = builder.start();
			int result = process.waitFor();
			if (result != 0)
			{
				String error = new String(process.getErrorStream().readAllBytes());
				String fullCommand = String.join(" ", command);
				throw new RuntimeException(
					"Compile filed with result code: " + result + System.lineSeparator() +
					"Command was: " + fullCommand + System.lineSeparator() +
					"Output:" + System.lineSeparator() + error
				);
			}
		}
		catch (IOException | InterruptedException exception)
		{
			throw new RuntimeException(exception);
		}
	}
}
