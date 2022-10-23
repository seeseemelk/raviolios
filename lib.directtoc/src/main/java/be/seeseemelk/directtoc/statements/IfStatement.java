package be.seeseemelk.directtoc.statements;

import be.seeseemelk.directtoc.CommonSyntax;
import be.seeseemelk.directtoc.expressions.Expression;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.Builder;
import lombok.Getter;
import lombok.experimental.SuperBuilder;

@Getter
@SuperBuilder
public class IfStatement implements Statement
{
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

	public IfStatement Then(Statement statement)
	{
		trueStatement = statement;
		return this;
	}

	public IfStatement Then(Statement... statements)
	{
		return Then( CommonSyntax.Block( statements ) );
	}

	public IfStatement Else(Statement statement)
	{
		falseStatement = statement;
		return this;
	}
}
