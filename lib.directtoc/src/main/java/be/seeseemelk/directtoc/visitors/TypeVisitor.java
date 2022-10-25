package be.seeseemelk.directtoc.visitors;

import be.seeseemelk.directtoc.SyntaxElement;
import be.seeseemelk.directtoc.types.Function;
import be.seeseemelk.directtoc.types.Pointer;
import be.seeseemelk.directtoc.types.Primitive;
import be.seeseemelk.directtoc.types.Type;
import lombok.Getter;

import java.util.HashSet;
import java.util.Set;

@Getter
public class TypeVisitor extends RecursiveVisitor
{
	private final Set<Type> types = new HashSet<>();

	@Override
	public void visitFunction(Function function)
	{
		types.add(function);
		types.add(function.getReturnType());
		super.visitFunction(function);
	}

	@Override
	public void visitPrimitive(Primitive primitive)
	{
		types.add(primitive);
	}

	@Override
	public void visitPointer(Pointer pointer)
	{
		types.add(pointer);
		super.visitPointer(pointer);
	}

	public static Set<Type> visit(SyntaxElement element)
	{
		TypeVisitor visitor = new TypeVisitor();
		element.visit(visitor);
		return visitor.types;
	}
}
