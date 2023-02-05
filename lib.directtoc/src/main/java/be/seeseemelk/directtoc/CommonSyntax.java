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

	public WhileStatement While(Expression expression)
	{
		return WhileStatement.builder()
			.expression(expression)
			.build();
	}

	public VariableDeclareStatement Declare(Variable variable)
	{
		return VariableDeclareStatement.builder()
			.variable(variable)
			.build();
	}

	public VariableDeclareStatement Declare(Variable variable, Expression initialiser)
	{
		return VariableDeclareStatement.builder()
			.variable(variable)
			.initialiser(initialiser)
			.build();
	}

	public AssignmentStatement Assign(Variable variable, Expression expression)
	{
		return AssignmentStatement.builder()
			.variable(variable)
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

	public CharLiteral Char(char value)
	{
		return new CharLiteral(value);
	}

	public LessThanOrEqualExpression LessThanOrEqual(Expression lesser, Expression greater)
	{
		return new LessThanOrEqualExpression(lesser, greater);
	}

	public GreaterThanExpression GreaterThan(Expression greater, Expression lesser)
	{
		return new GreaterThanExpression(greater, lesser);
	}

	public EqualExpression Equal(Expression left, Expression right)
	{
		return new EqualExpression(left, right);
	}

	public ReturnStatement Return(Expression expression)
	{
		return new ReturnStatement(expression);
	}

	public ReturnStatement Return()
	{
		return new ReturnStatement(null);
	}

	public AdditionExpression Add(Expression left, Expression right)
	{
		return new AdditionExpression(left, right);
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
