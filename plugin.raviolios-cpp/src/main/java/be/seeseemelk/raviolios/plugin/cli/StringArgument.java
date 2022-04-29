package be.seeseemelk.raviolios.plugin.cli;

import lombok.Getter;
import lombok.RequiredArgsConstructor;

@RequiredArgsConstructor
@Getter
public class StringArgument implements Argument
{
	private final String string;

	@Override
	public String toString()
	{
		return string;
	}
}
