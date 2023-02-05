package be.seeseemelk.directtoc;

import be.seeseemelk.directtoc.statements.IfStatement;
import be.seeseemelk.directtoc.expressions.Variable;
import be.seeseemelk.directtoc.types.Function;
import be.seeseemelk.directtoc.types.Type;
import be.seeseemelk.directtoc.visitors.*;
import be.seeseemelk.directtoc.visitors.c.CImplementationVisitor;
import be.seeseemelk.directtoc.visitors.c.CPrototypeVisitor;
import be.seeseemelk.directtoc.visitors.c.CVisitor;
import be.seeseemelk.directtoc.visitors.interpreter.Interpreter;
import be.seeseemelk.directtoc.visitors.interpreter.Value;
import org.junit.jupiter.api.Test;

import java.util.Set;

import static be.seeseemelk.directtoc.CommonSyntax.*;
import static be.seeseemelk.directtoc.CommonTypes.ptr;
import static be.seeseemelk.directtoc.expressions.BoolLiteral.TRUE;
import static be.seeseemelk.directtoc.types.Bool.BOOL;
import static be.seeseemelk.directtoc.types.Int.INT;
import static be.seeseemelk.directtoc.types.Void.VOID;
import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.containsInAnyOrder;
import static org.hamcrest.Matchers.equalTo;

public class ProgramTest
{
	@Test
	void testIf()
	{
		IfStatement element = If(TRUE);
		assertThat(CVisitor.toString(element), equalTo("if (true)\n{\n}"));

		assertThat(TypeVisitor.visit(element), containsInAnyOrder(BOOL));
	}

	@Test
	void testFunction()
	{
		Function function = Func(VOID, "myFunction").body();

		assertThat(CVisitor.toString(function), equalTo("myFunction"));
		assertThat(CPrototypeVisitor.toString(function), equalTo("static void myFunction(void);"));
		assertThat(CImplementationVisitor.toOutput(function), equalTo("""
			void myFunction(void)
			{
			}
			"""));

		assertThat(TypeVisitor.visit(function), containsInAnyOrder(function, VOID));
	}

	@Test
	void testProgram()
	{
		Function function =
			Func(VOID, "simpleIf").body(
				If(TRUE)
			);

		ProgramWriter program = new ProgramWriter();
		program.addFunction(function);
		assertThat(program.toC().toOutput(), equalTo("""
			#include "d2c.h"
			static void simpleIf(void);
			void simpleIf(void)
			{
				if (true)
				{
				}
			}
			"""));

		assertThat(TypeVisitor.visit(function), containsInAnyOrder(function, VOID, BOOL));
	}

	@Test
	void testPuts()
	{
		Function puts = Func(VOID, "puts").header("stdio.h").executeAs(() -> System.out.println("Called"));
		Function main = Func(VOID, "main").body(
			Expression( Call( puts ) )
		);

		ProgramWriter program = new ProgramWriter();
		program.addFunction(main);
		assertThat(program.toC().toOutput(), equalTo("""
			#include "d2c.h"
			#include <stdio.h>
			static void main(void);
			void main(void)
			{
				puts();
			}
			"""));

		Interpreter interpreter = new Interpreter();
		interpreter.call(main);
	}

	@Test
	void testGetLocation()
	{
		assertThat(Location.getLocation().getFile(), equalTo("ProgramTest.java"));
	}

	@Test
	void testRecursiveFactorial()
	{
		Function factorial = Func(INT, "factorial");
		Variable num = factorial.parameter(INT, "num");
		factorial.body(
			If(LessThanOrEqual(num, Int(1)))
				.Then(
					Return(num)
				)
				.Else(
					Return(Multiply(Call(factorial, Subtract(num, Int(1))), num))
				)
		);

		ProgramWriter program = new ProgramWriter();
		program.addFunction(factorial);
		assertThat(program.toC().toOutput(), equalTo("""
			#include "d2c.h"
			static int factorial(int num);
			int factorial(int num)
			{
				if (num < 1)
					return num;
				else
					return factorial(num - 1) * num;
			}
			"""));

		Set<Type> types = TypeVisitor.visit(factorial);
		assertThat(types, containsInAnyOrder(factorial, INT));

		Interpreter interpreter = new Interpreter();
		Value result = interpreter.call(factorial, 5);
		assertThat(result.asInt(), equalTo(120));
	}

	@Test
	void testPointers()
	{
		Function pointers = Func(INT, "testPointer");
		Variable pointer = pointers.parameter(ptr(INT), "pointer");
		pointers.body(
			Return( Deref(pointer) )
		);

		Set<Type> types = TypeVisitor.visit(pointers);
		assertThat(types, containsInAnyOrder(pointers, INT, ptr(INT)));

		ProgramWriter program = new ProgramWriter(pointers);
		assertThat(program.toC().toOutput(), equalTo("""
			#include "d2c.h"
			static int testPointer(int* pointer);
			int testPointer(int* pointer)
			{
				return *pointer;
			}
			"""));

		Interpreter interpreter = new Interpreter();
		Value value = Value.fromInt(23);
		Value result = interpreter.call(pointers, Value.pointerTo(value));
		assertThat(result.asInt(), equalTo(23));
	}

	@Test
	void testVariables()
	{
		Function function = Func(INT, "testVariables");
		Variable number = function.parameter(INT, "number");
		Variable sum = new Variable(INT, "sum");
		function.body(
			Declare( sum, Int(0) ),
			While( GreaterThan(number, Int(0)) ).Do(
				Assign( sum, Add(sum, Int(1)) ),
				Assign( number, Subtract(number, Int(1)) )
			),
			Return( sum )
		);

		ProgramWriter program = new ProgramWriter(function);
		assertThat(program.toC().toOutput(), equalTo("""
			#include "d2c.h"
			static int testVariables(int number);
			int testVariables(int number)
			{
				int sum = 0;
				while (number > 0)
				{
					sum = sum + 1;
					number = number - 1;
				}
				return sum;
			}
			"""));

		Interpreter interpreter = new Interpreter();
		Value value = Value.fromInt(23);
		Value result = interpreter.call(function, value);
		assertThat(result.asInt(), equalTo(23));
	}

	@Test
	void testEquality()
	{
		Function function = Func(INT, "testEqualTo42");
		Variable number = function.parameter(INT, "number");
		function.body(
			If( Equal(number, Int(42)) ).Then(
				Return( Int(1) )
			).Else(
				Return( Int(0) )
			)
		);

		ProgramWriter program = new ProgramWriter(function);
		assertThat(program.toC().toOutput(), equalTo("""
			#include "d2c.h"
			static int testEqualTo42(int number);
			int testEqualTo42(int number)
			{
				if (number == 42)
					return 1;
				else
					return 0;
			}
			"""));

		Interpreter interpreter = new Interpreter();
		assertThat(interpreter.call(function, Value.fromInt(5)).asInt(), equalTo(0));
		assertThat(interpreter.call(function, Value.fromInt(42)).asInt(), equalTo(1));
	}
}
