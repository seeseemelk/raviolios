package be.seeseemelk.directtoc.visitors.interpreter;

import be.seeseemelk.directtoc.SyntaxElement;
import be.seeseemelk.directtoc.expressions.*;
import be.seeseemelk.directtoc.statements.*;
import be.seeseemelk.directtoc.types.Function;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;

import java.util.*;

import static be.seeseemelk.directtoc.CommonTypes.VOID;

public class Interpreter
{
	private final VariableContext variables = new VariableContext();

	public Value call(Function function, Value... arguments)
	{
		for (int i = 0; i < function.getParameters().size(); i++)
		{
			Variable parameter = function.getParameters().get(i);
			Value argument = arguments[i];
			variables.create(parameter, argument);
		}

		return getValueOrNull(function);
	}

	public Value call(Function function, Object... arguments)
	{
		Value[] values = Arrays.stream(arguments)
			.map(Value::from)
			.toArray(Value[]::new);
		return call(function, values);
	}

	private Value getValue(SyntaxElement element)
	{
		return Objects.requireNonNull(getValueOrNull(element));
	}

	private Value getValueOrNull(SyntaxElement element)
	{
		InterpretingVisitor interpreter = new InterpretingVisitor();
		element.visit(interpreter);
		return interpreter.value;
	}

	private class InterpretingVisitor implements SyntaxVisitor
	{
		private Value value;

		// Types
		@Override
		public void visitFunction(Function function)
		{
			if (function.getExecuteAs() != null)
				function.getExecuteAs().run();
			else
				function.getBody().visit(this);
		}

		// Statements
		@Override
		public void visitBlockStatement(BlockStatement block)
		{
			variables.push();
			block.getStatements().forEach(statement -> statement.visit(this));
			variables.pop();
		}

		@Override
		public void visitExpressionStatement(ExpressionStatement statement)
		{
			statement.getExpression().visit(this);
		}

		@Override
		public void visitVariableDeclarationStatement(VariableDeclareStatement statement)
		{
			if (statement.getInitialiser() != null)
			{
				Value value = getValue(statement.getInitialiser());
				variables.create(statement.getVariable(), value);
			}
			else
			{
				variables.create(statement.getVariable(), NullValue.get());
			}
		}

		@Override
		public void visitIfStatement(IfStatement statement)
		{
			Value branch = getValue(statement.getExpression());
			if (branch.asBool())
				statement.getTrueStatement().visit(this);
			else if (statement.getFalseStatement() != null)
				statement.getFalseStatement().visit(this);
		}

		@Override
		public void visitWhileStatement(WhileStatement statement)
		{
			while (getValue(statement.getExpression()).asBool())
			{
				statement.getBody().visit(this);
			}
		}

		@Override
		public void visitAssignmentStatement(AssignmentStatement statement)
		{
			Variable variable = statement.getVariable();
			Value value = getValue(statement.getExpression());
			variables.update(variable, value);
		}

		@Override
		public void visitReturnStatement(ReturnStatement statement)
		{
			if (statement.getExpression() != null)
				value = getValue(statement.getExpression());
		}

		// Expressions
		@Override
		public void visitCall(CallExpression expression)
		{
			variables.push();
			List<Variable> parameters = expression.getFunction().getParameters();
			Expression[] arguments = expression.getArguments();
			for (int i = 0; i < parameters.size(); i++)
				variables.create(parameters.get(i), getValue(arguments[i]));
			if (expression.getResultType() != VOID)
				value = getValue(expression.getFunction());
			variables.pop();
		}

		@Override
		public void visitEqual(EqualExpression expression)
		{
			Value left = getValue(expression.getLeft());
			Value right = getValue(expression.getRight());
			value = Value.fromBool(left.asInt() == right.asInt());
		}

		@Override
		public void visitLessThanOrEqual(LessThanOrEqualExpression expression)
		{
			Value lesser = getValue(expression.getLesser());
			Value greater = getValue(expression.getGreater());
			value = Value.fromBool(lesser.asInt() <= greater.asInt());
		}

		@Override
		public void visitGreaterThan(GreaterThanExpression expression)
		{
			Value greater = getValue(expression.getGreater());
			Value lesser = getValue(expression.getLesser());
			value = Value.fromBool(greater.asInt() > lesser.asInt());
		}

		@Override
		public void visitMultiply(MultiplyExpression expression)
		{
			Value left = getValue(expression.getLeft());
			Value right = getValue(expression.getRight());
			value = Value.fromInt(left.asInt() * right.asInt());
		}

		@Override
		public void visitSubtract(SubtractExpression expression)
		{
			Value left = getValue(expression.getLeft());
			Value right = getValue(expression.getRight());
			value = Value.fromInt(left.asInt() - right.asInt());
		}

		@Override
		public void visitAddition(AdditionExpression expression)
		{
			Value left = getValue(expression.getLeft());
			Value right = getValue(expression.getRight());
			value = Value.fromInt(left.asInt() + right.asInt());
		}

		@Override
		public void visitIndex(IndexExpression expression)
		{
			Value pointer = getValue(expression.getVariable());
			Value index = getValue(expression.getIndex());
			if (index.asInt() != 0)
				throw new UnsupportedOperationException("Does not supported indexes other than 0 yet");
			value = pointer.deref();
		}

		@Override
		public void visitVariable(Variable variable)
		{
			value = variables.get(variable);
		}

		@Override
		public void visitIntLiteral(IntLiteral literal)
		{
			value = Value.fromInt(literal.getValue());
		}
	}

	private static class VariableContext
	{
		private Map<Variable, Value> values = new HashMap<>();
		private VariableContext parent;

		public boolean has(Variable variable)
		{
			return getOrNull(variable) != null;
		}

		public Value get(Variable variable)
		{
			Value value = getOrNull(variable);
			if (value == null)
				throw new IllegalArgumentException("Variable does not exist");
			return value;
		}

		public Value getOrNull(Variable variable)
		{
			if (values.containsKey(variable))
				return values.get(variable);
			else if (parent != null)
				return parent.get(variable);
			else
				return null;
		}

//		public void set(Variable variable, Value value)
//		{
//			if (!setIfPresent(variable, value))
//				create(variable, value);
//		}

		public void update(Variable variable, Value value)
		{
			if (!setIfPresent(variable, value))
				throw new IllegalArgumentException("Variable was not created");
		}

		public void create(Variable variable, Value value)
		{
			values.put(variable, value);
		}

		public boolean setIfPresent(Variable variable, Value value)
		{
			if (values.containsKey(variable))
			{
				values.put(variable, value);
				return true;
			}
			else if (parent != null)
				return parent.setIfPresent(variable, value);
			else
				return false;
		}

		public void push()
		{
			VariableContext newContext = new VariableContext();
			newContext.values = values;
			newContext.parent = parent;

			this.values = new HashMap<>();
			this.parent = newContext;
		}

		public void pop()
		{
			values = parent.values;
			parent = parent.parent;
		}
	}
}
