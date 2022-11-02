package be.seeseemelk.directtoc.statements;

import be.seeseemelk.directtoc.expressions.Expression;
import be.seeseemelk.directtoc.expressions.Variable;
import be.seeseemelk.directtoc.tags.Tags;
import be.seeseemelk.directtoc.types.Type;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.Builder;
import lombok.Data;

@Data
@Builder
public class VariableDeclareStatement implements Statement
{
	private final Tags tags = new Tags();
	private final Variable variable;
	private final Expression initialiser;

	public String getName()
	{
		return variable.getName();
	}

	public Type getType()
	{
		return variable.getType();
	}

	@Override
	public void visit(SyntaxVisitor visitor)
	{
		visitor.visitVariableDeclarationStatement(this);
	}

	public static class VariableDeclareStatementBuilder
	{
		public VariableDeclareStatementBuilder name(String name)
		{
			getVariable().setName(name);
			return this;
		}

		public VariableDeclareStatementBuilder type(Type type)
		{
			getVariable().setType(type);
			return this;
		}

		private Variable getVariable()
		{
			if (this.variable == null)
				this.variable = Variable.builder().build();
			return this.variable;
		}
	}
}
