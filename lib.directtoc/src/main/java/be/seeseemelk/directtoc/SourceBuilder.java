package be.seeseemelk.directtoc;

import java.util.ArrayList;
import java.util.List;

public class SourceBuilder
{
	private final List<String> lines = new ArrayList<>();
	private int indentation = 0;

	private String getIndentation()
	{
		return "\t".repeat(indentation);
	}

	public SourceBuilder write(String format, Object... args)
	{
		for (int i = 0; i < args.length; i++)
		{
			if (args[i] instanceof SourceBuilder builder)
			{
				args[i] = builder.toString();
			}
		}
		lines.add(getIndentation() + String.format(format, args));
		return this;
	}

	public SourceBuilder write(SourceBuilder source)
	{
		for (String line : source.lines)
		{
			write(line);
		}
		return this;
	}

	public SourceBuilder indent()
	{
		indentation++;
		return this;
	}

	public SourceBuilder indent(String format, Object... args)
	{
		write(format, args);
		return indent();
	}

	public SourceBuilder undent()
	{
		indentation--;
		return this;
	}

	public SourceBuilder undent(String format, Object... args)
	{
		undent();
		return write(format, args);
	}

	public SourceBuilder scope()
	{
		return indent("{");
	}

	public SourceBuilder unscope()
	{
		return undent("}");
	}

	public String toOutput()
	{
		return toString() + '\n';
	}

	@Override
	public String toString()
	{
		return String.join("\n", lines);
	}

	public static SourceBuilder of(String format, Object... args)
	{
		return new SourceBuilder().write(format, args);
	}
}
