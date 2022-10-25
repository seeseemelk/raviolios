package be.seeseemelk.directtoc;

import be.seeseemelk.directtoc.types.Function;
import be.seeseemelk.directtoc.types.Struct;
import be.seeseemelk.directtoc.types.Type;
import be.seeseemelk.directtoc.visitors.TypeVisitor;
import be.seeseemelk.directtoc.visitors.c.CImplementationVisitor;
import be.seeseemelk.directtoc.visitors.c.CPrototypeVisitor;
import be.seeseemelk.directtoc.visitors.c.IncludeVisitor;
import lombok.Data;
import lombok.RequiredArgsConstructor;

import java.util.HashSet;
import java.util.Set;
import java.util.function.Predicate;
import java.util.stream.Collectors;

@Data
@RequiredArgsConstructor
public class ProgramWriter
{
	private final Set<Type> usedTypes = new HashSet<>();
	private final Set<String> headers = new HashSet<>();
	private final String filename;

	public ProgramWriter()
	{
		this("d2c");
	}

	public ProgramWriter(Function function)
	{
		this();
		addFunction(function);
	}

	public void addFunction(Function type)
	{
		usedTypes.addAll(TypeVisitor.visit(type));
		headers.addAll(IncludeVisitor.visit(type));
	}

	public void addFunctions(Set<Function> functions)
	{
		functions.forEach(this::addFunction);
	}

	public Set<Type> getStructs()
	{
		return getTypesOf(type -> type instanceof Struct);
	}

	public Set<Type> getFunctions()
	{
		return getTypesOf(type -> type instanceof Function);
	}

	public SourceBuilder toC()
	{
		SourceBuilder builder = new SourceBuilder();
		builder.write("#include \"%s.h\"", filename);
		headers.forEach(header -> builder.write("#include <%s>", header));
		write(builder, getStructs(), CPrototypeVisitor::visit);
		write(builder, getStructs(), CImplementationVisitor::visit);
		write(builder, getFunctions(), CPrototypeVisitor::visit);
		write(builder, getFunctions(), CImplementationVisitor::visit);
		return builder;
	}

	public SourceBuilder toH()
	{
		String guard = "DIRECTTOC_" + filename;
		SourceBuilder builder = new SourceBuilder();
		builder.write("#ifndef %s", guard);
		builder.write("#define %s", guard);
		builder.write("");
		builder.write("#ifdef __cplusplus");
		builder.write("extern \"C\" {");
		builder.write("#endif");
		builder.write("");

		headers.forEach(header -> builder.write("#include <%s>", header));
		write(builder, getStructs(), CPrototypeVisitor::visit);
		write(builder, getStructs(), CImplementationVisitor::visit);

		builder.write("");
		builder.write("#ifdef __cplusplus");
		builder.write("}");
		builder.write("#endif");
		builder.write("");
		builder.write("#endif");
		return builder;
	}
	private void write(SourceBuilder builder, Set<Type> types, java.util.function.Function<Type, SourceBuilder> converter)
	{
		for (Type type : types)
		{
			builder.write(converter.apply(type));
		}
	}

	private Set<Type> getTypesOf(Predicate<Type> predicate)
	{
		return usedTypes.stream()
			.filter(predicate)
			.collect(Collectors.toSet());
	}
}
