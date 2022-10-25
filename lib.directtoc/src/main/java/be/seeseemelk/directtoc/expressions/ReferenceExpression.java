package be.seeseemelk.directtoc.expressions;

import be.seeseemelk.directtoc.tags.Tags;
import be.seeseemelk.directtoc.types.Type;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.Data;

@Data
public class ReferenceExpression implements Expression
{
	private final Tags tags = new Tags();
	private final Variable variable;

	@Override
	public void visit(SyntaxVisitor visitor)
	{
		visitor.visitReference(this);
	}

	@Override
	public Type getResultType()
	{
		return null;
	}
}
