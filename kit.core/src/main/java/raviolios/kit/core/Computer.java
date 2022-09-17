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

		int totalKb = memory.getTotalMemory() / 1024;
		StringBuilder builder = new StringBuilder("Total: ");
		builder.append(totalKb).append(" KiB");
		terminal.put(0, usageY, builder.toString());

		while (true)
		{
			int usedKb = memory.getUsedMemory() / 1024;
			builder = new StringBuilder("Used: ");
			builder.append(usedKb).append(" KiB");
			terminal.put(0, usageY - 1, builder.toString());

			terminal.put(0, 0, '\\');
			terminal.put(0, 0, '|');
			terminal.put(0, 0, '/');
			terminal.put(0, 0, '-');
		}
	}
}
