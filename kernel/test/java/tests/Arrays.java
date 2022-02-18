package tests;

import static raviolios.Asserts.assertEquals;

public class Arrays
{
	public static void canGetArrayLength() {
		char[] c = new char[5];
		assertEquals(5, c.length);
	}
}
