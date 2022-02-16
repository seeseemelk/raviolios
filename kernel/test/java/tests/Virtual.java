package tests;

import raviolios.Asserts;

public class Virtual
{
	private static class Parent
	{
		public void method()
		{
			Asserts.fail();
		}
	}

	private static class Child extends Parent
	{
		@Override
		public void method()
		{
			Asserts.success();
		}
	}

	public static void testCanCallVirtualMethod()
	{
		Parent obj = new Child();
		obj.method();
	}
}
