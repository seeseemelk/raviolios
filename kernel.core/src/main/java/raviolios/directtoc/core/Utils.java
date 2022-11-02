package raviolios.directtoc.core;

import be.seeseemelk.directtoc.expressions.BoolLiteral;
import be.seeseemelk.directtoc.expressions.Variable;
import be.seeseemelk.directtoc.types.Function;
import lombok.experimental.UtilityClass;

import static be.seeseemelk.directtoc.CommonSyntax.*;
import static be.seeseemelk.directtoc.CommonTypes.INT;
import static be.seeseemelk.directtoc.CommonTypes.ptr;
import static be.seeseemelk.directtoc.types.Char.CHAR;

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
	public static final Function stringLength = funcStringLength();

	private static Function funcStringLength()
	{
		Function function = Func(INT, "stringLength");
		Variable arg = function.parameter(ptr(CHAR), "string");
		Variable length = new Variable(INT, "length");
		return function.body(
			Declare(length, Int(0)),
			While(BoolLiteral.TRUE).Do(
				If(Equal(Index(arg, length), Char('\0'))).Then(
					Return(length)
				),
				Assign(length, Add(length, Int(1)))
			)
		);
	}
}
