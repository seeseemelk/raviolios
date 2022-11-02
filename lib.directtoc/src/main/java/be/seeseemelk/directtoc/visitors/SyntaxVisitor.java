package be.seeseemelk.directtoc.visitors;

import be.seeseemelk.directtoc.expressions.*;
import be.seeseemelk.directtoc.statements.*;
import be.seeseemelk.directtoc.types.Function;
import be.seeseemelk.directtoc.types.Pointer;
import be.seeseemelk.directtoc.types.Primitive;
import be.seeseemelk.directtoc.types.Struct;

public interface SyntaxVisitor
{
	// Types and function
	default void visitFunction(Function function)
	{
		throw new UnsupportedOperationException();
	}
	default void visitStruct(Struct struct)
	{
		throw new UnsupportedOperationException();
	}
	default void visitPrimitive(Primitive primitive)
	{
		throw new UnsupportedOperationException();
	}
	default void visitPointer(Pointer pointer)
	{
		throw new UnsupportedOperationException();
	}

	// Statements
	default void visitIfStatement(IfStatement statement)
	{
		throw new UnsupportedOperationException();
	}
	default void visitAssignmentStatement(AssignmentStatement statement)
	{
		throw new UnsupportedOperationException();
	}
	default void visitVariableDeclarationStatement(VariableDeclareStatement statement)
	{
		throw new UnsupportedOperationException();
	}
	default void visitBlockStatement(BlockStatement statement)
	{
		throw new UnsupportedOperationException();
	}
	default void visitExpressionStatement(ExpressionStatement statement)
	{
		throw new UnsupportedOperationException();
	}
	default void visitReturnStatement(ReturnStatement statement)
	{
		throw new UnsupportedOperationException();
	}
	default void visitWhileStatement(WhileStatement statement)
	{
		throw new UnsupportedOperationException();
	}
	default void visitCompoundStatement(CompoundStatement statement)
	{
		throw new UnsupportedOperationException("Compound statements should probably be transformed instead of visited");
	}

	// Expressions
	default void visitVariable(Variable variable)
	{
		throw new UnsupportedOperationException();
	}
	default void visitCall(CallExpression expression)
	{
		throw new UnsupportedOperationException();
	}
	default void visitLessThanOrEqual(LessThanOrEqualExpression expression)
	{
		throw new UnsupportedOperationException();
	}
	default void visitAddition(AdditionExpression expression)
	{
		throw new UnsupportedOperationException();
	}
	default void visitSubtract(SubtractExpression expression)
	{
		throw new UnsupportedOperationException();
	}
	default void visitMultiply(MultiplyExpression expression)
	{
		throw new UnsupportedOperationException();
	}
	default void visitIndex(IndexExpression expression)
	{
		throw new UnsupportedOperationException();
	}
	default void visitReference(ReferenceExpression expression)
	{
		throw new UnsupportedOperationException();
	}
	default void visitEqual(EqualExpression expression)
	{
		throw new UnsupportedOperationException();
	}

	// Literal expressions
	default void visitLiteral(Literal literal)
	{
		throw new UnsupportedOperationException();
	}
	default void visitBoolLiteral(BoolLiteral literal)
	{
		visitLiteral(literal);
	}
	default void visitIntLiteral(IntLiteral literal)
	{
		visitLiteral(literal);
	}
	default void visitCharLiteral(CharLiteral literal)
	{
		visitLiteral(literal);
	}
}
