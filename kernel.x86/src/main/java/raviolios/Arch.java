package raviolios;

public abstract class Arch
{
	public static native void trace(char chr);

	public static native int mapMemory(int address, int size);

	public static native void poke(int address, byte value);

	public static native int freeBytes();
	public static native int usedBytes();

	public static native int totalBytes();
}
