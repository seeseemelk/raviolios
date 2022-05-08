package tests;

import static raviolios.Asserts.assertEquals;

public class Strings
{
	public static void canGetCharacterAtPosition()
	{
		String str = "Hello";
		assertEquals('H', str.charAt(0));
		assertEquals('e', str.charAt(1));
		assertEquals('l', str.charAt(2));
		assertEquals('l', str.charAt(3));
		assertEquals('o', str.charAt(4));
	}
}
