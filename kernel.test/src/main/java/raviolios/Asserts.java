package raviolios;

public final class Asserts
{
	private Asserts() {}

	public static native void success();
	public static native void fail();

	public static void assertEquals(int expected, int actual)
	{
		if (expected == actual)
			success();
		else
			fail();
	}
}
