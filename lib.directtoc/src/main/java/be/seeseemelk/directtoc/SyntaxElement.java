package be.seeseemelk.directtoc;

import be.seeseemelk.directtoc.tags.Tags;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;

public interface SyntaxElement
{
	Tags getTags();

	void visit(SyntaxVisitor visitor);
}
