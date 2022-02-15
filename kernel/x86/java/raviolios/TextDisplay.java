package raviolios;

public class TextDisplay
{
	private final int screen;
	private final byte color = 0x06;

	public TextDisplay()
	{
		screen = Arch.mapMemory(0xB8000, 4096);
	}

	public final void putAt(char c, int x)
	{
		Arch.poke(x, (byte) c);
		Arch.poke(x + 1, color);
	}
}
