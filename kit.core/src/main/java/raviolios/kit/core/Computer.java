package raviolios.kit.core;

import lombok.Getter;
import lombok.Setter;
import raviolios.kit.core.devices.terminal.AbstractTerminalDevice;

public final class Computer
{
	@Getter
	@Setter
	private AbstractTerminalDevice terminal;

	@Getter
	@Setter
	private Memory memory;

	public void run()
	{
		int usageY = terminal.getHeight() - 1;

		terminal.put(0, usageY, "Usage");

		while (true)
		{
			terminal.put(0, 0, '\\');
			terminal.put(0, 0, '|');
			terminal.put(0, 0, '/');
			terminal.put(0, 0, '-');
		}
	}
}
