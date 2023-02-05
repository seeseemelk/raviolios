package be.seeseemelk.directtoc.visitors.c;

import be.seeseemelk.directtoc.SourceBuilder;
import be.seeseemelk.directtoc.SyntaxElement;
import be.seeseemelk.directtoc.expressions.*;
import be.seeseemelk.directtoc.statements.*;
import be.seeseemelk.directtoc.types.Function;
import be.seeseemelk.directtoc.types.Pointer;
import be.seeseemelk.directtoc.types.Primitive;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.Data;

import java.util.Arrays;
import java.util.stream.Collectors;

@Data
public class CVisitor implements SyntaxVisitor
{
	private final SourceBuilder source = new SourceBuilder();
	/**
	 * Contains the current precedence level.
	 * See: <a href="https://en.cppreference.com/w/c/language/operator_precedence">...</a>
	 * If an expression with an operator precedence higher than this one is used, brackets should be added
	 * around the expression.
	 */
	private final int operatorPrecedence;

	// Types
	@Override
	public void visitFunction(Function function)
	{
		source.write("%s", function.getName());
	}

	// Statements
	@Override
	public void visitBlockStatement(BlockStatement block)
	{
		source.scope();
		for (Statement statement : block.getStatements())
		{
			source.write(visit(statement));
		}
		source.unscope();
	}

	@Override
	public void visitExpressionStatement(ExpressionStatement statement)
	{
		source.write("%s;", visit(statement.getExpression()));
	}

	@Override
	public void visitVariableDeclarationStatement(VariableDeclareStatement statement)
	{
		SourceBuilder type = visit(statement.getVariable().getType());
		String name = statement.getVariable().getName();
		if (statement.getInitialiser() == null)
			source.write("%s %s;", type, name);
		else
			source.write("%s %s = %s;", type, name, visit(statement.getInitialiser()));
	}

	@Override
	public void visitIfStatement(IfStatement statement)
	{
		source.write("if (%s)", visit(statement.getExpression()));
		writeChild(statement.getTrueStatement());
		if (statement.getFalseStatement() != null)
		{
			source.write("else");
			writeChild(statement.getFalseStatement());
		}
	}

	@Override
	public void visitWhileStatement(WhileStatement statement)
	{
		source.write("while (%s)", visit(statement.getExpression()));
		writeChild(statement.getBody());
	}

	@Override
	public void visitReturnStatement(ReturnStatement statement)
	{
		if (statement.getExpression() != null)
			source.write("return %s;", visit(statement.getExpression()));
		else
			source.write("return;");
	}

	// Expressions

	@Override
	public void visitBoolLiteral(BoolLiteral literal)
	{
		source.write(literal.isState() ? "true" : "false");
	}

	@Override
	public void visitIntLiteral(IntLiteral literal)
	{
		source.write("%d", literal.getValue());
	}

	@Override
	public void visitCharLiteral(CharLiteral literal)
	{
		switch (literal.getValue())
		{
			case '\0':
				source.write("'\\0'");
				break;
			case '\n':
				source.write("'\\n'");
				break;
			case '\r':
				source.write("'\\r'");
				break;
			case '\t':
				source.write("'\\t'");
				break;
			default:
				source.write("'%s'", literal.getValue());
		}
	}

	@Override
	public void visitPrimitive(Primitive primitive)
	{
		source.write(primitive.getName());
	}

	@Override
	public void visitCall(CallExpression expression)
	{
		// Precedence 1
		String argumentList = Arrays.stream(expression.getArguments())
				.map(argument -> visit(argument).toString())
				.collect(Collectors.joining(", "));
		source.write("%s(%s)", visit(expression.getFunction(), 1), argumentList);
	}

	@Override
	public void visitVariable(Variable variable)
	{
		// Precedence 1
		if (variable.getParent() != null)
			source.write("%s.%s", visit(variable.getParent(), 1), variable.getName());
		else
			source.write("%s", variable.getName());
	}

	@Override
	public void visitAssignmentStatement(AssignmentStatement statement)
	{
		source.write("%s = %s;", visit(statement.getVariable()), visit(statement.getExpression()));
	}

	@Override
	public void visitEqual(EqualExpression expression)
	{
		visitBinaryExpression(expression.getLeft(), "==", expression.getRight(), 6);
	}

	@Override
	public void visitLessThanOrEqual(LessThanOrEqualExpression expression)
	{
		visitBinaryExpression(expression.getLesser(), "<=", expression.getGreater(), 6);
	}

	@Override
	public void visitGreaterThan(GreaterThanExpression expression)
	{
		visitBinaryExpression(expression.getGreater(), ">", expression.getLesser(), 6);
	}

	@Override
	public void visitAddition(AdditionExpression expression)
	{
		visitBinaryExpression(expression.getLeft(), "+", expression.getRight(), 4);
	}

	@Override
	public void visitSubtract(SubtractExpression expression)
	{
		visitBinaryExpression(expression.getLeft(), "-", expression.getRight(), 4);
	}

	@Override
	public void visitMultiply(MultiplyExpression expression)
	{
		visitBinaryExpression(expression.getLeft(), "*", expression.getRight(), 3);
	}

	private void visitBinaryExpression(Expression left, String operator, Expression right, int operatorPrecedence)
	{
		source.write(
			formatPrecedence("%s " + operator + " %s", operatorPrecedence),
			visit(left, operatorPrecedence),
			visit(right, operatorPrecedence)
		);
	}

	@Override
	public void visitPointer(Pointer pointer)
	{
		source.write(pointer.getName());
	}

	@Override
	public void visitIndex(IndexExpression expression)
	{
		if (expression.getTags().hasTag(CTags.IndexUsingPointerArithmetic.INSTANCE))
		{
			String index = visit(expression.getIndex(), 2).toString();
			if ("0".equals(index))
			{
				source.write(
					formatPrecedence("*%s", 2),
					visit(expression.getVariable(), 2)
				);
			}
			else
			{
				source.write(
					formatPrecedence("*(%s + %s)", 2),
					visit(expression.getVariable(), 2),
					index
				);
			}
		}
		else
		{
			source.write(
				formatPrecedence("%s[%s]", 1),
				visit(expression.getVariable(), 1),
				visit(expression.getIndex(), 1)
			);
		}
	}

	private String formatPrecedence(String format, int level)
	{
		if (operatorPrecedence < level)
			return "(" + format + ")";
		else
			return format;
	}

	private SourceBuilder writeChild(Statement statement)
	{
		SourceBuilder child = visit(statement);
		if (isBlock(statement))
			return source.write(child);
		else
			return source.indent().write(child).undent();
	}

	public static SourceBuilder visit(SyntaxElement element)
	{
		return visit(element, 15);
	}

	public static SourceBuilder visit(SyntaxElement element, int operatorPrecedence)
	{
		CVisitor visitor = new CVisitor(operatorPrecedence);
		element.visit(visitor);
		return visitor.source;
	}

	public static String toString(SyntaxElement element)
	{
		return visit(element).toString();
	}

	private static boolean isBlock(Statement statement)
	{
		return statement instanceof BlockStatement;
	}
}
