package be.seeseemelk.directtoc.statements;

import be.seeseemelk.directtoc.expressions.Variable;
import be.seeseemelk.directtoc.expressions.Expression;
import be.seeseemelk.directtoc.types.Type;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.Data;
import lombok.experimental.SuperBuilder;

@Data
@SuperBuilder
public class VariableDeclareStatement implements Statement
{
	private final String name;
	private final Type type;
	private final Expression initialiser;

	@Override
	public void visit(SyntaxVisitor visitor)
	{
		visitor.visitVariableDeclarationStatement(this);
	}

	public Variable access()
	{
		return Variable.builder()
			.name(name)
			.type(type)
			.build();
	}
}
