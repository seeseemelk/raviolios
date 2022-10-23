package be.seeseemelk.directtoc.visitors.c;

import be.seeseemelk.directtoc.SourceBuilder;
import be.seeseemelk.directtoc.SyntaxElement;
import be.seeseemelk.directtoc.types.Function;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.Data;

@Data
public class CImplementationVisitor implements SyntaxVisitor
{
	private final SourceBuilder source = new SourceBuilder();

	@Override
	public void visitFunction(Function function)
	{
		if (function.getBody() != null)
		{
			SourceBuilder returnType = CVisitor.visit(function.getReturnType());
			source.write("%s %s(%s)", returnType, function.getName(), CPrototypeVisitor.formatParameterList(function));
			source.write(CVisitor.visit(function.getBody()));
		}
	}

	public static SourceBuilder visit(SyntaxElement element)
	{
		CImplementationVisitor visitor = new CImplementationVisitor();
		element.visit(visitor);
		return visitor.source;
	}

	public static String toString(SyntaxElement element)
	{
		return visit(element).toString();
	}

	public static String toOutput(SyntaxElement element)
	{
		return visit(element).toOutput();
	}
}
