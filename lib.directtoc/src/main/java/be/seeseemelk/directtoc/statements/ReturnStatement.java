package be.seeseemelk.directtoc.statements;

import be.seeseemelk.directtoc.expressions.Expression;
import be.seeseemelk.directtoc.tags.Tags;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.Data;

@Data
public class ReturnStatement implements Statement
{
	private final Tags tags = new Tags();
	private final Expression expression;

	@Override
	public void visit(SyntaxVisitor visitor)
	{
		visitor.visitReturnStatement(this);
	}
}
