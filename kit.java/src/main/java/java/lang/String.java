package java.lang;

public final class String
{
	private final char[] characters;

	public String(char[] characters)
	{
		this.characters = characters;
	}

	public int length()
	{
		return characters.length;
	}

	public char charAt(int i)
	{
		return characters[i];
	}

	public static String fromUtf8(byte[] bytes)
	{
		char[] chars = new char[bytes.length];
		for (int i = 0; i < chars.length; i++)
			chars[i] = (char) bytes[i];
		return new String(chars);
	}

	public static String copyValueOf(char[] characters)
	{
		return new String(Arrays.copyOf(characters, characters.length));
	}

	public static String valueOf(int value)
	{
		if (value == 0)
			return "0";

		StringBuilder builder = new StringBuilder();
		boolean negative = value < 0;
		while (value > 0)
		{
			builder.append((char) ('0' + (value % 10)));
			value /= 10;
		}
		if (negative)
			builder.append('-');
		return builder.reverse().toString();
	}

	public static String valueOf(char[] characters)
	{
		return copyValueOf(characters);
	}
}
