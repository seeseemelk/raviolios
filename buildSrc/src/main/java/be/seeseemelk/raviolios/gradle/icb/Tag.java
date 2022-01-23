package be.seeseemelk.raviolios.gradle.icb;

public enum Tag
{
	CLASS(0x01);

	private final byte tag;

	Tag(int tag)
	{
		this.tag = (byte) tag;
	}

	public byte asByte()
	{
		return tag;
	}
}
