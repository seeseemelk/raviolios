package be.seeseemelk.directtoc.expressions;

import be.seeseemelk.directtoc.tags.Tags;
import be.seeseemelk.directtoc.types.Type;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.Builder;
import lombok.Getter;
import lombok.experimental.SuperBuilder;

@Getter
@SuperBuilder
public class Variable implements Expression
{
	private final Tags tags = new Tags();
	private final Type type;
	private final String name;

	@Builder.Default
	private Variable parent = null;

	@Override
	public void visit(SyntaxVisitor visitor)
	{
		visitor.visitVariable(this);
	}

	@Override
	public Type getResultType()
	{
		return type;
	}
}
