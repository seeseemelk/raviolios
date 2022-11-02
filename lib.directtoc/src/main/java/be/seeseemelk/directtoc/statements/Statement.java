package be.seeseemelk.directtoc.statements;

import be.seeseemelk.directtoc.CommonSyntax;
import be.seeseemelk.directtoc.SyntaxElement;

import java.util.Arrays;
import java.util.stream.Stream;

public interface Statement extends SyntaxElement
{
	static Statement[] transform(Statement... statements)
	{
		return Arrays.stream(statements)
			.flatMap(Statement::decomposeCompound)
			.toArray(Statement[]::new);
	}

	static Statement transformToSingle(Statement... statements)
	{
		statements = transform(statements);
		if (statements.length == 1)
			return statements[0];
		else
			return CommonSyntax.Block(statements);
	}

	private static Stream<Statement> decomposeCompound(Statement statement)
	{
		if (statement instanceof CompoundStatement compound)
			return Arrays.stream(compound.getStatements());
		else
			return Stream.of(statement);
	}
}
