package be.seeseemelk.directtoc.visitors;

import be.seeseemelk.directtoc.expressions.*;
import be.seeseemelk.directtoc.statements.*;
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
	public void visitWhileStatement(WhileStatement statement)
	{
		statement.getExpression().visit(this);
		statement.getBody().visit(this);
	}

	@Override
	public void visitAssignmentStatement(AssignmentStatement statement)
	{
		statement.getVariable().visit(this);
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
	public void visitVariableDeclarationStatement(VariableDeclareStatement statement)
	{
		statement.getVariable().visit(this);
		if (statement.getInitialiser() != null)
			statement.getInitialiser().visit(this);
	}

	@Override
	public void visitPrimitive(Primitive primitive)
	{
	}

	@Override
	public void visitEqual(EqualExpression expression)
	{
		expression.getLeft().visit(this);
		expression.getRight().visit(this);
	}

	@Override
	public void visitLessThanOrEqual(LessThanOrEqualExpression expression)
	{
		expression.getLesser().visit(this);
		expression.getGreater().visit(this);
	}

	@Override
	public void visitGreaterThan(GreaterThanExpression expression)
	{
		expression.getGreater().visit(this);
		expression.getLesser().visit(this);
	}

	@Override
	public void visitVariable(Variable variable)
	{
		variable.getType().visit(this);
		if (variable.getParent() != null)
			variable.getParent().visit(this);
	}

	@Override
	public void visitAddition(AdditionExpression expression)
	{
		expression.getLeft().visit(this);
		expression.getRight().visit(this);
	}

	@Override
	public void visitSubtract(SubtractExpression expression)
	{
		expression.getLeft().visit(this);
		expression.getRight().visit(this);
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
