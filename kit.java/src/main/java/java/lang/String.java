package java.lang;

public final class String
{
	private final char[] characters;

	public String(char[] characters)
	{
		this.characters = characters;
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
}
