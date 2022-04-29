package be.seeseemelk.raviolios.plugin.cli;

import org.apache.commons.lang3.StringUtils;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

public class Arguments implements Cloneable
{
	private final List<Argument> arguments;

	public Arguments()
	{
		arguments = new ArrayList<>();
	}

	public Arguments(Arguments base)
	{
		arguments = new ArrayList<>(base.getArguments());
	}

	public Arguments add(Argument argument)
	{
		arguments.add(argument);
		return this;
	}

	public Arguments add(String string)
	{
		return add(new StringArgument(string));
	}

	public Arguments addSource(File file)
	{
		return add(new SourceFileArgument(file));
	}

	public Arguments addOutput(File file)
	{
		return add(new OutputFileArgument(file));
	}

	public Arguments addAll(Arguments argumentsToAdd)
	{
		arguments.addAll(argumentsToAdd.getArguments());
		return this;
	}

	public Arguments addAll(List<String> arguments)
	{
		arguments.forEach(this::add);
		return this;
	}

	public List<Argument> getArguments()
	{
		return arguments;
	}

	public List<String> getArgumentsString()
	{
		return arguments.stream()
			.map(Argument::toString)
			.collect(Collectors.toList());
	}

	@Override
	public String toString()
	{
		return StringUtils.join(" ", getArgumentsString());
	}
}
