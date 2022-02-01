package tests;

import raviolios.Asserts;

public class StaticProperty
{
	private static int propInt = 0;
	public static void propertyStartsAsZero()
	{
		Asserts.assertEquals(0, propInt);
	}

	public static void propertyCanBeChanged()
	{
		propInt = 42;
		Asserts.assertEquals(42, propInt);
		propInt = 0;
		Asserts.assertEquals(0, propInt);
	}
}
