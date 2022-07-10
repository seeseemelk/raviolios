package tests;

import static raviolios.Asserts.fail;
import static raviolios.Asserts.success;

public class Conditionals
{
	public static int getNumber()
	{
		return 0;
	}

	public static void testIf()
	{
		if (getNumber() == 0)
		{
			success();
		}
		else
		{
			fail();
		}
	}

	public static void testIfNot()
	{
		if (getNumber() != 0)
		{
			fail();
		}
		else
		{
			success();
		}
	}
}
