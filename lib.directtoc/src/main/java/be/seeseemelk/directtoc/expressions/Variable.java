package be.seeseemelk.directtoc.expressions;

import be.seeseemelk.directtoc.tags.Tags;
import be.seeseemelk.directtoc.types.Type;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Getter;
import lombok.Setter;

@Setter
@Getter
@Builder
@AllArgsConstructor
public class Variable implements Expression
{
	private final Tags tags = new Tags();
	private Type type;
	private String name;
	@Builder.Default
	private Variable parent = null;

	public Variable(Type type, String name)
	{
		this.type = type;
		this.name = name;
	}

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
