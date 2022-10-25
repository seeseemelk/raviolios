package be.seeseemelk.directtoc;

import be.seeseemelk.directtoc.types.Void;
import be.seeseemelk.directtoc.types.*;
import lombok.experimental.UtilityClass;

@UtilityClass
public class CommonTypes
{
	public static final Bool BOOL = Bool.BOOL;
	public static final Int INT = Int.INT;
	public static final Void VOID = Void.VOID;

	public static Pointer ptr(Type type)
	{
		return new Pointer(type);
	}
}
