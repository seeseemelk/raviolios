package tests;

import static raviolios.Asserts.assertEquals;

public class Arithmetic
{
	public void canAddNumbers()
	{
		int value = 5;
		assertEquals(5, value);
		assertEquals(6, value + 1);
	}

	public void canIncrementNumbers()
	{
		int value = 3;
		assertEquals(3, value);
		value++;
		assertEquals(4, value);
	}
}
