package be.seeseemelk.directtoc.visitors.interpreter;

import lombok.Getter;
import lombok.Setter;
import lombok.ToString;

@Getter
@Setter
@ToString
public class Value
{
	public static Value NULL;
	public static Value TRUE = fromBool(true);
	public static Value FALSE = fromBool(false);

	static
	{
		NULL = new Value();
		NULL.isNull = true;
	}

	private boolean isNull = false;
	private Integer integer = null;
	private Boolean bool = null;
	private Value pointer = null;

	private Value() {}

	public int asInt()
	{
		if (isNull())
			return 0;
		if (integer == null)
			throw new IllegalStateException("Not an int");
		return integer;
	}

	public boolean asBool()
	{
		if (isNull())
			return false;
		if (bool == null)
			throw new IllegalStateException("Not a bool");
		return bool;
	}

	public Value deref()
	{
		if (isNull())
			throw new IllegalStateException("Null-pointer");
		return pointer;
	}

	public static Value uninitialised()
	{
		return new Value();
	}

	public static Value fromInt(int value)
	{
		Value result = new Value();
		result.integer = value;
		return result;
	}

	public static Value fromBool(boolean value)
	{
		Value result = new Value();
		result.bool = value;
		return result;
	}

	public static Value pointerTo(Value value)
	{
		Value result = new Value();
		result.pointer = value;
		return result;
	}
	
	public static Value from(Object object)
	{
		if (object instanceof Integer value)
			return fromInt(value);
		else
			throw new IllegalArgumentException("Unsupported object type " + object.getClass().getSimpleName());
	}
}
