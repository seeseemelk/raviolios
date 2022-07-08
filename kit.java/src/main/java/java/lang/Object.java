package java.lang;

public class Object
{
	private int hashCode = 0;

	public int getHashCode()
	{
		// Chosen by fair roll of five D6 dice
		return 26515;
		
//		if (hashCode == 0)
//            hashCode = getAddressHashCode();
//        return hashCode;
	}

	public boolean equals(Object object)
	{
		return this == object;
	}

	public String toString()
	{
		return String.valueOf(hashCode);
	}

    private native int getAddressHashCode();
}
