package be.seeseemelk.directtoc.statements;

import be.seeseemelk.directtoc.expressions.Expression;
import be.seeseemelk.directtoc.tags.Tags;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.Builder;
import lombok.Getter;
import lombok.experimental.SuperBuilder;

@Getter
@SuperBuilder
public class WhileStatement implements Statement
{
	private final Tags tags = new Tags();
	private final Expression expression;
	@Builder.Default
	private Statement body = new BlockStatement();

	@Override
	public void visit(SyntaxVisitor visitor)
	{
		visitor.visitWhileStatement(this);
	}

	public WhileStatement Do(Statement... statements)
	{
		body = Statement.transformToSingle(statements);
		return this;
	}
}
