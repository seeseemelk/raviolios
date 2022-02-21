package tests;

import static raviolios.Asserts.assertEquals;

public class Objects
{
	private static class Obj
	{
		public int value = 5;
	}

	public static void canReadObjectProperty()
	{
		Obj object = new Obj();
		assertEquals(5, object.value);
	}
}
