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

	public static void assertSame(Object expected, Object actual)
	{
		if (expected == actual)
			success();
		else
			fail();
	}

	public static void assertNotSame(Object expected, Object actual)
	{
		if (expected != actual)
			success();
		else
			fail();
	}

	public static void assertNotNull(Object actual)
	{
		if (actual != null)
			success();
		else
			fail();
	}

	public static void assertNull(Object actual)
	{
		if (actual == null)
			success();
		else
			fail();
	}
}
