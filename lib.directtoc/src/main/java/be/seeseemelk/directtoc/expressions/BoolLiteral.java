package be.seeseemelk.directtoc.expressions;

import be.seeseemelk.directtoc.tags.Tags;
import be.seeseemelk.directtoc.types.Type;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.*;

import static be.seeseemelk.directtoc.types.Bool.BOOL;

@Getter
@AllArgsConstructor(access = AccessLevel.PRIVATE)
public class BoolLiteral implements Literal
{
	public static final BoolLiteral TRUE = new BoolLiteral(true);
	public static final BoolLiteral FALSE = new BoolLiteral(false);
	private final boolean state;
	private final Tags tags = new Tags();

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
