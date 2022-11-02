package be.seeseemelk.directtoc.expressions;

import be.seeseemelk.directtoc.tags.Tags;
import be.seeseemelk.directtoc.types.Type;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.Data;

@Data
public class EqualExpression implements Expression
{
	private final Tags tags = new Tags();
	private final Expression left;
	private final Expression right;

	@Override
	public void visit(SyntaxVisitor visitor)
	{
		visitor.visitEqual(this);
	}

	@Override
	public Type getResultType()
	{
		return null;
	}
}
