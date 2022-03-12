package raviolios.kit.core;

import lombok.Data;
import lombok.Getter;
import lombok.Setter;
import raviolios.kit.core.devices.terminal.AbstractTerminalDevice;

public final class Computer
{
	@Getter
	@Setter
	private AbstractTerminalDevice terminal;

	public void run()
	{
		while (true)
		{
			terminal.put(0, 0, '/');
			terminal.put(0, 0, '-');
			terminal.put(0, 0, '\\');
			terminal.put(0, 0, '|');
		}
	}
}
