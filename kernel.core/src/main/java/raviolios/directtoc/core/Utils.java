package raviolios.directtoc.core;

import be.seeseemelk.directtoc.types.Function;
import lombok.experimental.UtilityClass;

import static be.seeseemelk.directtoc.CommonSyntax.Func;
import static be.seeseemelk.directtoc.CommonTypes.INT;

@UtilityClass
public class Utils
{
	/*
	size_t stringLength(const char* string)
	{
		size_t length = 0;
		while (true)
		{
			if (string[length] == '\0')
				return length;
			length++;
		}
	}
	 */
	public static final Function stringLength;

	static
	{
		stringLength = Func(INT, "stringLength");
	}
}
