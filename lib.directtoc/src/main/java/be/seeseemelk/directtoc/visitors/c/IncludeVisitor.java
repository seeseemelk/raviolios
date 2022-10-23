package be.seeseemelk.directtoc.visitors.c;

import be.seeseemelk.directtoc.SyntaxElement;
import be.seeseemelk.directtoc.types.Function;
import be.seeseemelk.directtoc.visitors.RecursiveVisitor;

import java.util.HashSet;
import java.util.Set;

public class IncludeVisitor extends RecursiveVisitor
{
	private final Set<String> headers = new HashSet<>();

	@Override
	public void visitFunction(Function function)
	{
		if (function.getHeader() != null)
			headers.add(function.getHeader());
		super.visitFunction(function);
	}

	public static Set<String> visit(SyntaxElement element)
	{
		IncludeVisitor visitor = new IncludeVisitor();
		element.visit(visitor);
		return visitor.headers;
	}
}
