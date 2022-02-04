package tests;

import static raviolios.Asserts.assertEquals;

public class Variables
{
	public void canSaveVariables()
	{
		int value = 123;
		assertEquals(123, value);
		value = 1337;
		assertEquals(1337, value);
		int varTwo = 1337;
		assertEquals(value, varTwo);
	}
}
