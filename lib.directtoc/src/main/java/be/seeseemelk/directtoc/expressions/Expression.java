package be.seeseemelk.directtoc.expressions;

import be.seeseemelk.directtoc.CommonSyntax;
import be.seeseemelk.directtoc.SyntaxElement;
import be.seeseemelk.directtoc.statements.ExpressionStatement;
import be.seeseemelk.directtoc.types.Type;

public interface Expression extends SyntaxElement
{
	Type getResultType();

	default ExpressionStatement asStatement()
	{
		return CommonSyntax.Expression(this);
	}
}
