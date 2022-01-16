package raviolios.java.lang;

/**
 * The root of all objects.
 */
public class Object
{
	private int hashCode = 0;

	public int hashCode()
	{
		if (this.hashCode == 0)
			this.hashCode = getAddressHash();
		return this.hashCode;
	}

	private native int getAddressHash();
}