package be.seeseemelk.directtoc.visitors.c;

import be.seeseemelk.directtoc.SourceBuilder;
import be.seeseemelk.directtoc.SyntaxElement;
import be.seeseemelk.directtoc.expressions.Variable;
import be.seeseemelk.directtoc.types.Function;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;

public class HeaderVisitor implements SyntaxVisitor
{
	private final SourceBuilder source = new SourceBuilder();

	@Override
	public void visitFunction(Function function)
	{
		if (function.getBody() != null)
		{
			SourceBuilder returnType = CVisitor.visit(function.getReturnType());
			if (function.isExported())
				source.write("%s %s(%s);", returnType, function.getName(), CPrototypeVisitor.formatParameterList(function));
		}
	}

	@Override
	public void visitVariable(Variable variable)
	{
		source.write("%s %s", CVisitor.visit(variable.getType()), CVisitor.visit(variable));
	}

	public static SourceBuilder visit(SyntaxElement element)
	{
		HeaderVisitor visitor = new HeaderVisitor();
		element.visit(visitor);
		return visitor.source;
	}

	public static String toString(SyntaxElement element)
	{
		return visit(element).toString();
	}
}
