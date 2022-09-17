package tests;

import static raviolios.Asserts.*;

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

	public int getLength(int extra, String text)
	{
		return text.length() + extra;
	}

	public static void testStringLiteralInMethod()
	{
		Strings stringsA = new Strings();
		int length = stringsA.getLength(0, "hello");
		assertEquals(5, length);
		assertNotNull(stringsA);
	}
}
