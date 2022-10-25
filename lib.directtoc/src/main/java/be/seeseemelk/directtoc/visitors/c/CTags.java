package be.seeseemelk.directtoc.visitors.c;

import be.seeseemelk.directtoc.tags.Tag;
import lombok.experimental.UtilityClass;

@UtilityClass
public class CTags
{
	public static class IndexUsingPointerArithmetic implements Tag
	{
		public static final IndexUsingPointerArithmetic INSTANCE = new IndexUsingPointerArithmetic();

		private IndexUsingPointerArithmetic() {}
	}
}
