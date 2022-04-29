package be.seeseemelk.raviolios.plugin.cli;

public interface Argument
{
}
/*
public class Argument
{
	private String asString;
	private File asFile;

	public Argument(String string)
	{
		asString = string;
	}

	public Argument(File file)
	{
		asFile = file;
	}

	public boolean isFile()
	{
		return asFile != null;
	}

	public String getString()
	{
		return Objects.requireNonNull(asString);
	}

	public File getFile()
	{
		return Objects.requireNonNull(asFile);
	}

	@Override
	public String toString()
	{
		if (isFile())
			return asFile.getAbsolutePath();
		else
			return asString;
	}
}
*/
