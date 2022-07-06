package tests;

import raviolios.Asserts;

public class Loops
{
	private static final int ITERATIONS = 1_000_000;
	private static int value = 0;

	public static void longForLoop()
	{
		for (int i = 0; i < ITERATIONS; i++)
			value++;
		Asserts.assertEquals(ITERATIONS, value);
	}
}
