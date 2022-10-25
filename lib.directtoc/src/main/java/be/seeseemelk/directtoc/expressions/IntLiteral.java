package be.seeseemelk.directtoc.expressions;

import be.seeseemelk.directtoc.tags.Tags;
import be.seeseemelk.directtoc.types.Type;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.Data;
import lombok.EqualsAndHashCode;

import static be.seeseemelk.directtoc.CommonTypes.INT;

@Data
@EqualsAndHashCode(callSuper = false)
public class IntLiteral implements Literal
{
	private final int value;
	private final Tags tags = new Tags();

	@Override
	public void visit(SyntaxVisitor visitor)
	{
		visitor.visitIntLiteral(this);
	}

	@Override
	public Type getResultType()
	{
		return INT;
	}
}
