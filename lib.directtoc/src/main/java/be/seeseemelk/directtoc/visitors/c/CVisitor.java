package be.seeseemelk.directtoc.visitors.c;

import be.seeseemelk.directtoc.SourceBuilder;
import be.seeseemelk.directtoc.SyntaxElement;
import be.seeseemelk.directtoc.expressions.*;
import be.seeseemelk.directtoc.statements.*;
import be.seeseemelk.directtoc.types.Function;
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
	 *
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
	public void visitLessThanOrEqual(LessThanOrEqualExpression expression)
	{
		source.write(
			formatPrecendence("%s < %s", 6),
			visit(expression.getSmaller(), 6),
			visit(expression.getBigger(), 6)
		);
	}

	@Override
	public void visitSubtract(SubtractExpression expression)
	{
		source.write(
			formatPrecendence("%s - %s", 4),
			visit(expression.getLeft(), 4),
			visit(expression.getRight(), 4)
		);
	}

	@Override
	public void visitMultiply(MultiplyExpression expression)
	{
		source.write(
			formatPrecendence("%s * %s", 3),
			visit(expression.getLeft(), 3),
			visit(expression.getRight(), 3)
		);
	}

	private String formatPrecendence(String format, int level)
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