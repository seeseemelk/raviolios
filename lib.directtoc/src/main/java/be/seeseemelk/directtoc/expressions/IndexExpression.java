package be.seeseemelk.directtoc.expressions;

import be.seeseemelk.directtoc.types.Type;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.Data;

@Data
public class IndexExpression implements Expression
{
	private final Variable variable;
	private final Expression index;

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
