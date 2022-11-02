package be.seeseemelk.directtoc.statements;

import be.seeseemelk.directtoc.expressions.Expression;
import be.seeseemelk.directtoc.tags.Tags;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.Builder;
import lombok.Getter;
import lombok.experimental.SuperBuilder;

@Getter
@SuperBuilder
public class IfStatement implements Statement
{
	private final Tags tags = new Tags();
	private final Expression expression;
	@Builder.Default
	private Statement trueStatement = new BlockStatement();
	@Builder.Default
	private Statement falseStatement = null;

	@Override
	public void visit(SyntaxVisitor visitor)
	{
		visitor.visitIfStatement(this);
	}

	public IfStatement Then(Statement... statements)
	{
		trueStatement = Statement.transformToSingle(statements);
		return this;
	}

	public IfStatement Else(Statement... statements)
	{
		falseStatement = Statement.transformToSingle(statements);
		return this;
	}
}
