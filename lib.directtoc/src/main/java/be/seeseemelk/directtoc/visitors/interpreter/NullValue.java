package be.seeseemelk.directtoc.visitors.interpreter;

public class NullValue extends Value
{
	private static final NullValue INSTANCE = new NullValue();

	private NullValue() {}

	public static NullValue get()
	{
		return INSTANCE;
	}
}
