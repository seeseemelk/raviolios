package be.seeseemelk.directtoc.statements;

import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.Data;
import lombok.Getter;

import java.util.ArrayList;
import java.util.List;

@Getter
public class BlockStatement implements Statement
{
	private final List<Statement> statements = new ArrayList<>();

	public BlockStatement add(Statement element)
	{
		statements.add(element);
		return this;
	}

	public BlockStatement add(List<Statement> elements)
	{
		this.statements.addAll(elements);
		return this;
	}

	public void visit(SyntaxVisitor visitor)
	{
		visitor.visitBlockStatement(this);
	}
}
