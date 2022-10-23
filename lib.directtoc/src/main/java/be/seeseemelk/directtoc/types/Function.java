package be.seeseemelk.directtoc.types;

import be.seeseemelk.directtoc.CommonSyntax;
import be.seeseemelk.directtoc.expressions.Variable;
import be.seeseemelk.directtoc.statements.BlockStatement;
import be.seeseemelk.directtoc.statements.Statement;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.Builder;
import lombok.Data;
import lombok.ToString;

import java.util.ArrayList;
import java.util.List;

@Data
@Builder
public class Function implements Type
{
	private final List<Variable> parameters = new ArrayList<>();

	@ToString.Include
	private String name;
	private BlockStatement body;
	private String header;
	private Type returnType;
	private Runnable executeAs;
	private boolean exported;

	@Override
	public void visit(SyntaxVisitor visitor)
	{
		visitor.visitFunction(this);
	}

	public Function body(BlockStatement block)
	{
		this.body = block;
		return this;
	}

	public Function body(Statement... statements)
	{
		return body(CommonSyntax.Block(statements));
	}

	public Function header(String header)
	{
		setHeader(header);
		return this;
	}

	public Function executeAs(Runnable runnable)
	{
		setExecuteAs(runnable);
		return this;
	}

	public Function parameter(Variable variable)
	{
		parameters.add(variable);
		return this;
	}

	public Function export()
	{
		exported = true;
		return this;
	}

	public Variable parameter(Type type, String name)
	{
		Variable variable = Variable.builder()
			.name(name)
			.type(type)
			.build();
		parameter(variable);
		return variable;
	}
}
