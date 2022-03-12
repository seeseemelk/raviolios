package raviolios.devices.terminal;

import raviolios.Arch;
import raviolios.kit.core.devices.terminal.AbstractTerminalDevice;

public class TextModeTerminalDevice extends AbstractTerminalDevice
{
	private int screen;
	private static final byte COLOR = 0x06;

	public TextModeTerminalDevice()
	{
		screen = Arch.mapMemory(0xB8000, 4096);
	}

	@Override
	public String getName()
	{
		return "TextModeTerminal";
	}

	@Override
	public int getWidth()
	{
		return 80;
	}

	@Override
	public int getHeight()
	{
		return 40;
	}

	@Override
	public void put(int x, int y, char character)
	{
		int ptr = screen + x + (y * getWidth());
		Arch.poke(ptr, (byte) character);
		Arch.poke(ptr, COLOR);
	}
}
