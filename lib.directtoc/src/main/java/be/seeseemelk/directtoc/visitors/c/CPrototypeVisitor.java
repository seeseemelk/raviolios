package be.seeseemelk.directtoc.visitors.c;

import be.seeseemelk.directtoc.SourceBuilder;
import be.seeseemelk.directtoc.SyntaxElement;
import be.seeseemelk.directtoc.expressions.Variable;
import be.seeseemelk.directtoc.types.Function;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.Data;

import java.util.stream.Collectors;

@Data
public class CPrototypeVisitor implements SyntaxVisitor
{
	private final SourceBuilder source = new SourceBuilder();

	@Override
	public void visitFunction(Function function)
	{
		if (function.getBody() != null)
		{
			SourceBuilder returnType = CVisitor.visit(function.getReturnType());
			String format = "%s %s(%s);";
			if (!function.isExported())
				format = "static " + format;
			source.write(format, returnType, function.getName(), formatParameterList(function));
		}
	}

	@Override
	public void visitVariable(Variable variable)
	{
		source.write("%s %s", CVisitor.visit(variable.getType()), CVisitor.visit(variable));
	}

	public static SourceBuilder visit(SyntaxElement element)
	{
		CPrototypeVisitor visitor = new CPrototypeVisitor();
		element.visit(visitor);
		return visitor.source;
	}

	public static String formatParameterList(Function function)
	{
		if (function.getParameters().isEmpty())
			return "void";
		else
			return function.getParameters().stream()
				.map(CPrototypeVisitor::visit)
				.map(SourceBuilder::toString)
				.collect(Collectors.joining(", "));
	}

	public static String toString(SyntaxElement element)
	{
		return visit(element).toString();
	}
}
