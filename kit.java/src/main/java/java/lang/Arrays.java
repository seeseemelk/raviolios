package java.lang;

public class Arrays
{
	public static char[] copyOf(char[] array, int length)
	{
		char[] newArray = new char[length];
		int copyLength = Math.min(array.length, length);
		for (int i = 0; i < copyLength; i++)
			newArray[i] = array[i];
		return newArray;
	}
}
