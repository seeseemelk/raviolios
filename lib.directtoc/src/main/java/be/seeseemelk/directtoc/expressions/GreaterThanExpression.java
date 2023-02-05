package be.seeseemelk.directtoc.expressions;

import be.seeseemelk.directtoc.tags.Tags;
import be.seeseemelk.directtoc.types.Type;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.Data;

@Data
public class GreaterThanExpression implements Expression
{
	private final Tags tags = new Tags();
	private final Expression greater;
	private final Expression lesser;

	@Override
	public void visit(SyntaxVisitor visitor)
	{
		visitor.visitGreaterThan(this);
	}

	@Override
	public Type getResultType()
	{
		return null;
	}
}
