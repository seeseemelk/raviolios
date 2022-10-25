package be.seeseemelk.directtoc.types;

import be.seeseemelk.directtoc.tags.Tags;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.AccessLevel;
import lombok.Data;
import lombok.RequiredArgsConstructor;

@Data
@RequiredArgsConstructor(access = AccessLevel.PROTECTED)
public abstract class Primitive implements Type
{
	private final Tags tags = new Tags();
	private final String name;

	@Override
	public void visit(SyntaxVisitor visitor)
	{
		visitor.visitPrimitive(this);
	}

	//	@Override
//	public SourceBuilder toC()
//	{
//		return SourceBuilder.of(name);
//	}
//
//	@Override
//	public SourceBuilder toPrototypeC()
//	{
//		return new SourceBuilder();
//	}
//
//	@Override
//	public SourceBuilder toImplementationC()
//	{
//		return new SourceBuilder();
//	}

//	@Override
//	public Set<Type> getUsedTypes()
//	{
//		return Set.of(this);
//	}
}
