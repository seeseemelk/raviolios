package tests;

import static raviolios.Asserts.assertEquals;
import static raviolios.Asserts.assertSame;

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

	private static class ChildObject
	{
		public int number;
	}

	private static class ParentObject
	{
		public Object object;
	}

	public static void canStoredObjectsAsProperties()
	{
		ChildObject object1 = new ChildObject();
		ChildObject object2 = new ChildObject();
		ParentObject parent = new ParentObject();

		object1.number = 12;
		object2.number = 50;

		parent.object = object1;

		assertSame(object1, parent.object);
		assertEquals(12, object1.number);
	}
}
