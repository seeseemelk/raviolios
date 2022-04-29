package be.seeseemelk.raviolios.plugin.cli.local;

import be.seeseemelk.raviolios.plugin.cli.Arguments;
import be.seeseemelk.raviolios.plugin.cli.CommandRunner;
import be.seeseemelk.raviolios.plugin.cli.OutputFileArgument;
import lombok.RequiredArgsConstructor;
import org.gradle.workers.WorkerExecutor;

@RequiredArgsConstructor
public class LocalRunner implements CommandRunner
{
	private final WorkerExecutor executor;

	@Override
	public void run(Arguments command)
	{
		executor.noIsolation().submit(CppBuildWorkAction.class, parameters -> {
			parameters.getCommand().set(command.getArgumentsString());
			command.getArguments().stream()
				.filter(argument -> argument instanceof OutputFileArgument)
				.map(argument -> ((OutputFileArgument) argument).getFile())
				.findAny()
				.ifPresent(output -> parameters.getOutput().set(output));
		});
	}

	@Override
	public void close() throws Exception
	{
		executor.await();
	}

	@Override
	public void await()
	{
		executor.await();
	}
}
