package be.seeseemelk.directtoc;

import lombok.Data;

@Data
public class Location
{
	private final String file;
	private final int line;

	public static Location getLocation(int stackDepth)
	{
		StackTraceElement element = Thread.currentThread().getStackTrace()[stackDepth];
		return new Location(element.getFileName(), element.getLineNumber());
	}

	public static Location getLocation()
	{
		return getLocation(3);
	}

	public static Location getLocationOfCaller()
	{
		return getLocation(4);
	}
}
