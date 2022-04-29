package be.seeseemelk.raviolios.plugin.cli;

import lombok.Getter;
import lombok.RequiredArgsConstructor;

import java.io.File;

@RequiredArgsConstructor
@Getter
public class FileArgument implements Argument
{
	private final File file;

	@Override
	public String toString()
	{
		return file.getAbsolutePath();
	}
}
