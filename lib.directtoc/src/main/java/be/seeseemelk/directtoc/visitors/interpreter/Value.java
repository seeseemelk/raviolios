package be.seeseemelk.directtoc.visitors.interpreter;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public abstract class Value
{
	public int asInt()
	{
		throw new IllegalStateException("Not an int");
	}

	public boolean asBool()
	{
		throw new IllegalStateException("Not a bool");
	}

	public Value deref()
	{
		throw new IllegalStateException("Not a pointer");
	}

	public static IntValue fromInt(int value)
	{
		return new IntValue(value);
	}

	public static BoolValue fromBool(boolean value)
	{
		return value ? BoolValue.TRUE : BoolValue.FALSE;
	}

	public static ArrayValue pointerTo(Value value)
	{
		return arrayOf(value);
	}

	public static ArrayValue arrayOf(Value... values)
	{
		return new ArrayValue(values, 0);
	}
	
	public static Value from(Object object)
	{
		if (object instanceof Integer value)
			return fromInt(value);
		else
			throw new IllegalArgumentException("Unsupported object type " + object.getClass().getSimpleName());
	}
}
