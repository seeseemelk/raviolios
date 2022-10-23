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
}
