package java.lang;

public class Enum<E extends Enum<E>>
{
	private final String name;
	private final int ordinal;

	protected Enum(String name, int ordinal)
	{
		this.name = name;
		this.ordinal = ordinal;
	}

	public String name()
	{
		return this.name;
	}

	public int ordinal()
	{
		return this.ordinal;
	}

	public static <T extends Enum<T>> T valueOf(Class<T> enumType, String name)
	{
		return null;
	}
}
