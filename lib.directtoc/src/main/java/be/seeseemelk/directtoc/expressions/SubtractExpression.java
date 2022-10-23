package be.seeseemelk.directtoc.expressions;

import be.seeseemelk.directtoc.types.Type;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.Data;

@Data
public class SubtractExpression implements Expression
{
	private final Expression left;
	private final Expression right;

	@Override
	public void visit(SyntaxVisitor visitor)
	{
		visitor.visitSubtract(this);
	}

	@Override
	public Type getResultType()
	{
		return left.getResultType();
	}
}
