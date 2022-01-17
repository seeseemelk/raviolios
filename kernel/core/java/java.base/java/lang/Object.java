package java.lang;

public class Object {
	private int hashCode = 0;

	public int getHashCode()
	{
		if (hashCode == 0)
            hashCode = getAddressHashCode();
        return hashCode;
	}

    private native int getAddressHashCode();
}
