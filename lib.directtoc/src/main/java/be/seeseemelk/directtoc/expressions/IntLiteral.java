package be.seeseemelk.directtoc.expressions;

import be.seeseemelk.directtoc.types.Type;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.*;

import static be.seeseemelk.directtoc.CommonTypes.INT;

@Data
@EqualsAndHashCode(callSuper = false)
public class IntLiteral extends Literal
{
	private final int value;

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
