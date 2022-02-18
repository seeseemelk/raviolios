package raviolios;

public final class Startup
{
	private Startup() {}

	public static void main()
	{
		TextDisplay display = new TextDisplay();
		int x = 0;
		display.putAt('B', x++);
		display.putAt('o', x++);
		display.putAt('o', x++);
		display.putAt('t', x++);
		display.putAt('e', x++);
		display.putAt('d', x++);
		display.putAt(' ', x++);
		display.putAt('f', x++);
		display.putAt('r', x++);
		display.putAt('o', x++);
		display.putAt('m', x++);
		display.putAt(' ', x++);
		display.putAt('J', x++);
		display.putAt('a', x++);
		display.putAt('v', x++);
		display.putAt('a', x++);
		display.putAt(' ', x++);
		display.putAt(' ', x++);
		display.putAt(' ', x++);
	}
}
