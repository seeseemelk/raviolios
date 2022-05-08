package tests;

import static raviolios.Asserts.assertSame;

public class Methods
{
	private static Object returnObject(Object object)
	{
		return object;
	}

	public static void canReturnReference()
	{
		Object obj = new Object();
		Object ref = returnObject(obj);
		assertSame(obj, returnObject(ref));
	}
}
