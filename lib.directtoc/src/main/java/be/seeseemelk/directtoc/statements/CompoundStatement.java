package be.seeseemelk.directtoc.statements;

import be.seeseemelk.directtoc.tags.Tags;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.Getter;
import lombok.RequiredArgsConstructor;

@Getter
@RequiredArgsConstructor
public class CompoundStatement implements Statement
{
	private final Tags tags = new Tags();
	private final Statement[] statements;

	@Override
	public void visit(SyntaxVisitor visitor)
	{
		throw new UnsupportedOperationException("Compound statements cannot be visited, they should be transformed");
	}
}
