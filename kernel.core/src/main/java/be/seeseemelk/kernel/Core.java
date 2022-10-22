package be.seeseemelk.kernel;

import be.seeseemelk.directtoc.ProgramWriter;
import be.seeseemelk.directtoc.types.Function;

import static be.seeseemelk.directtoc.CommonSyntax.Func;
import static be.seeseemelk.directtoc.CommonTypes.VOID;

public class Core
{
	public static void main(String[] args)
	{
		Function test = Func(VOID, "test");

		ProgramWriter writer = new ProgramWriter();
		writer.addFunction(test);
	}
}
