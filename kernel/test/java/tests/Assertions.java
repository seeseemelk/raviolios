package tests;

import raviolios.Asserts;

public class Assertions
{
	/*private static void indirection1()
	{
		Asserts.success();
	}

	private static void indirection2()
	{
		indirection1();
	}*/

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
