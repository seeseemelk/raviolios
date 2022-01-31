package tests;

import raviolios.Asserts;

public class StaticProperty
{
	private static int intProp = 0;
	public static void intPropIsZero()
	{
		Asserts.assertEquals(0, intProp);
	}

	private static int intPropNonZero = 42;
	public static void intPropNonZeroIsNotZero()
	{
		Asserts.assertEquals(42, intPropNonZero);
		Asserts.assertEquals(0, intPropNonZero);
	}
}
