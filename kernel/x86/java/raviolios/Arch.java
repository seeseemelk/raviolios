package raviolios;

public abstract class Arch
{
	public static native void printHelloWorld();

	public static native void trace(char chr);

	public static native void mapMemory(int address, int size);
}
