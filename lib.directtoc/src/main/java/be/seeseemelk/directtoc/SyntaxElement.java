package be.seeseemelk.directtoc;

import be.seeseemelk.directtoc.visitors.SyntaxVisitor;

public interface SyntaxElement
{
//	Set<Type> getUsedTypes();

	void visit(SyntaxVisitor visitor);

//	SourceBuilder toC();
}
