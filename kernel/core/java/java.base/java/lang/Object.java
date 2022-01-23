package java.lang;

public class Object
{
	private int hashCode = 0;

	public int getHashCode()
	{
		if (hashCode == 0)
            hashCode = getAddressHashCode();
        return hashCode;
	}

	public boolean equals(Object object)
	{
		return this == object;
	}

    private native int getAddressHashCode();
}
