package be.seeseemelk.directtoc.expressions;

import be.seeseemelk.directtoc.types.Type;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.*;

import static be.seeseemelk.directtoc.types.Bool.BOOL;

@Getter
@AllArgsConstructor(access = AccessLevel.PRIVATE)
public class BoolLiteral extends Literal
{
	public static final BoolLiteral TRUE = new BoolLiteral(true);
	public static final BoolLiteral FALSE = new BoolLiteral(false);

	private final boolean state;

	@Override
	public Type getResultType()
	{
		return BOOL;
	}

	@Override
	public void visit(SyntaxVisitor visitor)
	{
		visitor.visitBoolLiteral(this);
	}
}
