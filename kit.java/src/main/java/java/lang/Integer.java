package java.lang;

public class Integer
{
	public static String toString(int i)
	{
		if (i == 0)
			return "0";
		char[] str = new char[lengthInDecimal(i)];
		int writeIndex = str.length;
		while (i != 0)
		{
			str[--writeIndex] = (char) ((i % 10) + '0');
			i /= 10;
		}
		return new String(str);
	}

	private static int lengthInDecimal(int i)
	{
		int length = 1;
		if (i < 0)
		{
			length++;
			i = -(i + 1);
		}
		while (i != 0)
		{
			length++;
			i /= 10;
		}
		return length;
	}
}
