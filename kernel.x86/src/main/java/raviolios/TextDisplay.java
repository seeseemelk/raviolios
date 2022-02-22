package raviolios;

public class TextDisplay
{
	private final int screen;
	private final byte color = 0x06;

	public TextDisplay()
	{
		screen = Arch.mapMemory(0xB8000, 4096);
	}

	private void noOp() {}

	public void putAt(char c, int x)
	{
		Arch.poke(screen + x * 2, (byte) c);
		Arch.poke(screen + x * 2 + 1, color);
	}
}
