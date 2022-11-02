package be.seeseemelk.directtoc.visitors.interpreter;

import lombok.AllArgsConstructor;

@AllArgsConstructor
public class ArrayValue extends Value
{
	private Value[] values;
	private int offset;

	public Value deref()
	{
		return deref(0);
	}

	public Value deref(int index)
	{
		index += offset;
		if (values == null)
			throw new NullPointerException("Value is null");
		if (index >= values.length)
			throw new BufferOverflowException("Offset too large");
		if (index < 0)
			throw new BufferOverflowException("Offset too small");
		return values[index];
	}
}
