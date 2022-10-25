package be.seeseemelk.directtoc;

import be.seeseemelk.directtoc.expressions.*;
import be.seeseemelk.directtoc.statements.*;
import be.seeseemelk.directtoc.types.Function;
import be.seeseemelk.directtoc.types.Type;
import be.seeseemelk.directtoc.visitors.c.CTags;
import lombok.experimental.UtilityClass;

import java.util.Arrays;

@UtilityClass
public class CommonSyntax
{
	public Function Func(Type returnType, String name)
	{
		return Function.builder()
			.returnType(returnType)
			.name(name)
			.build();
	}

	public BlockStatement Block(Statement... elements)
	{
		return Block().add(Arrays.asList(elements));
	}

	public BlockStatement Block()
	{
		return new BlockStatement();
	}

	public IfStatement If(Expression expression)
	{
		return IfStatement.builder()
			.expression(expression)
			.build();
	}

	public ExpressionStatement Expression(Expression expression)
	{
		return new ExpressionStatement(expression);
	}

	public CallExpression Call(Function function, Expression... arguments)
	{
		return new CallExpression(function, arguments);
	}

	public IntLiteral Int(int value)
	{
		return new IntLiteral(value);
	}

	public LessThanOrEqualExpression LessThanOrEqual(Expression smaller, Expression bigger)
	{
		return new LessThanOrEqualExpression(smaller, bigger);
	}

	public ReturnStatement Return(Expression expression)
	{
		return new ReturnStatement(expression);
	}

	public ReturnStatement Return()
	{
		return new ReturnStatement(null);
	}

	public SubtractExpression Subtract(Expression left, Expression right)
	{
		return new SubtractExpression(left, right);
	}

	public MultiplyExpression Multiply(Expression left, Expression right)
	{
		return new MultiplyExpression(left, right);
	}

	public IndexExpression Index(Variable var, Expression index)
	{
		return new IndexExpression(var, index);
	}

	public IndexExpression Deref(Variable var)
	{
		IndexExpression expression = Index(var, Int(0));
		expression.getTags().addTag(CTags.IndexUsingPointerArithmetic.INSTANCE);
		return expression;
	}
}
