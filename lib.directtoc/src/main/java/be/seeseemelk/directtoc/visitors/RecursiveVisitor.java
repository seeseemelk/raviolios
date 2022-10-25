package be.seeseemelk.directtoc.visitors;

import be.seeseemelk.directtoc.expressions.*;
import be.seeseemelk.directtoc.statements.BlockStatement;
import be.seeseemelk.directtoc.statements.ExpressionStatement;
import be.seeseemelk.directtoc.statements.IfStatement;
import be.seeseemelk.directtoc.statements.ReturnStatement;
import be.seeseemelk.directtoc.types.Function;
import be.seeseemelk.directtoc.types.Pointer;
import be.seeseemelk.directtoc.types.Primitive;

public abstract class RecursiveVisitor implements SyntaxVisitor
{
	// Types

	@Override
	public void visitFunction(Function function)
	{
		function.getReturnType().visit(this);
		if (function.getBody() != null)
			function.getBody().visit(this);
		function.getParameters().forEach(parameter -> parameter.visit(this));
	}

	// Statements
	@Override
	public void visitBlockStatement(BlockStatement block)
	{
		block.getStatements().forEach(statement -> statement.visit(this));
	}

	@Override
	public void visitIfStatement(IfStatement statement)
	{
		statement.getExpression().visit(this);
	}

	@Override
	public void visitReturnStatement(ReturnStatement statement)
	{
		if (statement.getExpression() != null)
			statement.getExpression().visit(this);
	}

	// Expressions
	@Override
	public void visitExpressionStatement(ExpressionStatement statement)
	{
		statement.getExpression().visit(this);
	}

	@Override
	public void visitLiteral(Literal literal)
	{
		literal.getResultType().visit(this);
	}

	@Override
	public void visitCall(CallExpression expression)
	{
		expression.getFunction().visit(this);
	}

	@Override
	public void visitPrimitive(Primitive primitive)
	{
	}

	@Override
	public void visitLessThanOrEqual(LessThanOrEqualExpression expression)
	{
		expression.getSmaller().visit(this);
		expression.getBigger().visit(this);
	}

	@Override
	public void visitVariable(Variable variable)
	{
		variable.getType().visit(this);
		if (variable.getParent() != null)
			variable.getParent().visit(this);
	}

	@Override
	public void visitPointer(Pointer pointer)
	{
		pointer.getOf().visit(this);
	}

	@Override
	public void visitIndex(IndexExpression expression)
	{
		expression.getVariable().visit(this);
		expression.getIndex().visit(this);
	}
}
