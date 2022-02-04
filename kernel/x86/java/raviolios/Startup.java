package raviolios;

public final class Startup
{
	private Startup() {}

	public static void main()
	{
		int screen = Arch.mapMemory(0xB8000, 4096);
		int offset = 0;
		byte color = 0x06;
		Arch.poke(screen++, (byte) 'T'); Arch.poke(screen++, color);
		Arch.poke(screen++, (byte) 'h'); Arch.poke(screen++, color);
		Arch.poke(screen++, (byte) 'i'); Arch.poke(screen++, color);
		Arch.poke(screen++, (byte) 's'); Arch.poke(screen++, color);
		Arch.poke(screen++, (byte) ' '); Arch.poke(screen++, color);

		Arch.poke(screen++, (byte) 'i'); Arch.poke(screen++, color);
		Arch.poke(screen++, (byte) 's'); Arch.poke(screen++, color);
		Arch.poke(screen++, (byte) ' '); Arch.poke(screen++, color);

		Arch.poke(screen++, (byte) 'J'); Arch.poke(screen++, color);
		Arch.poke(screen++, (byte) 'a'); Arch.poke(screen++, color);
		Arch.poke(screen++, (byte) 'v'); Arch.poke(screen++, color);
		Arch.poke(screen++, (byte) 'a'); Arch.poke(screen++, color);
		Arch.poke(screen++, (byte) '!'); Arch.poke(screen++, color);
		Arch.poke(screen++, (byte) ' '); Arch.poke(screen++, color);
		Arch.poke(screen++, (byte) ' '); Arch.poke(screen++, color);
		Arch.poke(screen++, (byte) ' '); Arch.poke(screen++, color);
		Arch.poke(screen++, (byte) ' '); Arch.poke(screen++, color);
		Arch.poke(screen++, (byte) ' '); Arch.poke(screen++, color);
		Arch.poke(screen++, (byte) ' '); Arch.poke(screen++, color);

		Arch.trace('j');
		Arch.trace('a');
		Arch.trace('v');
		Arch.trace('a');
	}
}
