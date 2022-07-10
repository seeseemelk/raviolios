package tests;

import static raviolios.Asserts.assertEquals;

public class Arithmetic
{
	public static void canAddNumbers()
	{
		int value = 5;
		assertEquals(5, value);
		assertEquals(6, value + 1);
	}

	public static void canSubtract()
	{
		int a = 10;
		int b = 7;
		assertEquals(3, a - b);
	}

	public static void canIncrementNumbers()
	{
		int value = 3;
		assertEquals(3, value);
		value++;
		assertEquals(4, value);
	}

	public static void canMultiply()
	{
		int a = 2;
		int b = 3;
		assertEquals(6, a * b);
	}

	public static void canDivide()
	{
		int a = 10;
		int b = 2;
		assertEquals(5, a / b);
	}

	public static void canRemainder()
	{
		int a = 10;
		int b = 7;
		assertEquals(3, a % b);
	}
}
