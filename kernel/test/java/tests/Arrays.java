package tests;

import static raviolios.Asserts.assertEquals;

public class Arrays
{
	public static void canGetArrayLength() {
		char[] c = new char[5];
		assertEquals(5, c.length);
	}

	public static void canReadAndWriteCharArrays() {
		char[] c = new char[5];
		c[0] = 'a';
		c[1] = 'b';
		c[2] = 'c';
		c[3] = 'd';
		assertEquals('a', c[0]);
		assertEquals('b', c[1]);
		assertEquals('c', c[2]);
		assertEquals('d', c[3]);
		assertEquals('\0', c[4]);
	}
}
