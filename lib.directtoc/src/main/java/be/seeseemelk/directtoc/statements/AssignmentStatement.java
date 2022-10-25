package be.seeseemelk.directtoc.statements;

import be.seeseemelk.directtoc.expressions.Expression;
import be.seeseemelk.directtoc.expressions.Variable;
import be.seeseemelk.directtoc.tags.Tags;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.Data;
import lombok.experimental.SuperBuilder;

@Data
@SuperBuilder
public class AssignmentStatement implements Statement
{
	private final Tags tags = new Tags();
	private Variable variable;
	private Expression expression;

	@Override
	public void visit(SyntaxVisitor visitor)
	{
		visitor.visitAssignmentStatement(this);
	}
}
