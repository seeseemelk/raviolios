package tests;

import static raviolios.Asserts.assertEquals;

public class Constants
{
	private static int field;
	public static void canUseLargeIntegers() {
		assertEquals(0, field);
		field = 0xDEADBEEF;
		assertEquals(0xDEADBEEF, field);
	}
}
