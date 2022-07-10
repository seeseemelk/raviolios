package tests;

import static raviolios.Asserts.*;

public class Objects
{
	private static class Obj
	{
		public int value = 5;

		public void copyFrom(ChildObject other)
		{
			this.value = other.number;
		}
	}

	public static void canReadObjectProperty()
	{
		Obj object = new Obj();
		assertEquals(5, object.value);
	}

	private static class ChildObject
	{
		public int number;

		public int getNumber()
		{
			return number;
		}

		public void setNumber(int number)
		{
			this.number = number;
		}

		public void addAndSetNumber(int a, int b)
		{
			setNumber(a + b);
		}
	}

	private static class ParentObject
	{
		public Object object;
	}

	private static class ParentsChildObject extends ParentObject
	{
		private ChildObject child;
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

	public static void inheritanceTreeCanHaveMultipleObjects()
	{
		ParentsChildObject pco = new ParentsChildObject();
		Obj o = new Obj();

		pco.object = o;
		pco.child = new ChildObject();
		pco.child.number = 10;

		assertEquals(((Obj) pco.object).value, 5);
		assertEquals(pco.child.number, 10);
	}

	public static void methodsWithParameters()
	{
		ChildObject obj = new ChildObject();
		obj.number = 3;
		assertEquals(3, obj.number);
		obj.setNumber(10);
		assertEquals(10, obj.getNumber());
	}

	public static void methodsWithMultipleParameters()
	{
		ChildObject obj = new ChildObject();
		obj.number = 0;
		obj.addAndSetNumber(10, 3);
		assertEquals(13, obj.number);

		Obj other = new Obj();
		other.copyFrom(obj);
		assertEquals(13, other.value);
	}

	private final static class MultiObject
	{
		Object objectA;
		Object objectB;
	}

	public static void testMultiObject()
	{
		MultiObject multi = new MultiObject();
		ChildObject objectA = new ChildObject();
		String objectB = "a string";

		multi.objectA = objectA;
		multi.objectB = objectB;

		assertSame(objectA, multi.objectA);
		//assertSame(objectB, multi.objectB);
		//success();
	}
}
