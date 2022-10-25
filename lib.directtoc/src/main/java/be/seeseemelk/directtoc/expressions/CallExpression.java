package be.seeseemelk.directtoc.expressions;

import be.seeseemelk.directtoc.tags.Tags;
import be.seeseemelk.directtoc.types.Function;
import be.seeseemelk.directtoc.types.Type;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.Data;

@Data
public class CallExpression implements Expression
{
	private final Function function;
	private final Expression[] arguments;
	private final Tags tags = new Tags();

	@Override
	public void visit(SyntaxVisitor visitor)
	{
		visitor.visitCall(this);
	}

	@Override
	public Type getResultType()
	{
		return function.getReturnType();
	}
}
