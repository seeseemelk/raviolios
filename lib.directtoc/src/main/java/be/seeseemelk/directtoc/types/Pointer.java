package be.seeseemelk.directtoc.types;

import be.seeseemelk.directtoc.tags.Tags;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.Data;

@Data
public class Pointer implements Type
{
	private final Tags tags = new Tags();
	private final Type of;

	@Override
	public void visit(SyntaxVisitor visitor)
	{
		visitor.visitPointer(this);
	}

	@Override
	public String getName()
	{
		return of.getName() + "*";
	}
}
