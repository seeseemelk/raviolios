package tests;

import raviolios.Asserts;

public class Assertions
{
	public static void succeeds()
	{
		Asserts.success();
	}

	public static void fails()
	{
		Asserts.fail();
	}

	public static void equalsSucceedsWhenSame()
	{
		Asserts.assertEquals(1337, 1337);
	}

	public static void equalsFailsWhenDifferent()
	{
		Asserts.assertEquals(1337, 42);
	}
}
