package be.seeseemelk.directtoc.expressions;

import be.seeseemelk.directtoc.tags.Tags;
import be.seeseemelk.directtoc.types.Type;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.Data;
import lombok.EqualsAndHashCode;

import static be.seeseemelk.directtoc.CommonTypes.CHAR;

@Data
@EqualsAndHashCode
public class CharLiteral implements Literal
{
	private final Tags tags = new Tags();
	private final char value;

	@Override
	public void visit(SyntaxVisitor visitor)
	{
		visitor.visitCharLiteral(this);
	}

	@Override
	public Type getResultType()
	{
		return CHAR;
	}
}
