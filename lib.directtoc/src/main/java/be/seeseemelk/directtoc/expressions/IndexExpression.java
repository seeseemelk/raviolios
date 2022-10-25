package be.seeseemelk.directtoc.expressions;

import be.seeseemelk.directtoc.tags.Tags;
import be.seeseemelk.directtoc.types.Type;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.Data;

@Data
public class IndexExpression implements Expression
{
	private final Variable variable;
	private final Expression index;
	private final Tags tags = new Tags();

	@Override
	public void visit(SyntaxVisitor visitor)
	{
		visitor.visitIndex(this);
	}

	@Override
	public Type getResultType()
	{
		return variable.getResultType();
	}
}
