package raviolios;

public abstract class Startup
{
	public static native void printHelloWorld();

	public static void startup()
	{
		printHelloWorld();
	}
}
