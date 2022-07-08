package java.lang;

public class StringBuilder
{
	private char[] string;
	private int length = 0;

	public StringBuilder()
	{
		this(16);
	}

	public StringBuilder(int capacity)
	{
		string = new char[capacity];
	}

	public void ensureCapacity(int capacity)
	{
		if (string.length < capacity)
		{
			string = Arrays.copyOf(string, capacity);
		}
	}

	public StringBuilder append(char chr)
	{
		ensureCapacity(length + 1);
		string[length++] = chr;
		return this;
	}

	public StringBuilder reverse()
	{
		for (int i = 0; i < string.length / 2; i++)
		{
			char tmp = string[i];
			string[i] = string[string.length - i - 1];
			string[string.length - i - 1] = tmp;
		}
		return this;
	}

	@Override
	public String toString()
	{
		return String.valueOf(string);
	}
}
