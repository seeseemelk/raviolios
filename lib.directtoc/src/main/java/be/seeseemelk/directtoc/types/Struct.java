package be.seeseemelk.directtoc.types;

import be.seeseemelk.directtoc.expressions.Variable;
import be.seeseemelk.directtoc.tags.Tags;
import be.seeseemelk.directtoc.visitors.SyntaxVisitor;
import lombok.AllArgsConstructor;
import lombok.Data;

import java.util.ArrayList;
import java.util.List;

@Data
public class Struct implements Type
{
	private final Tags tags = new Tags();
	private final String name;
	private final List<Field> fields = new ArrayList<>();

	public Struct field(Field field)
	{
		fields.add(field);
		return this;
	}

	public Struct field(String name, Type type)
	{
		fields.add(new Field(this, name, type));
		return this;
	}

	public Variable access(Variable parent, String field)
	{
		return getField(field).access(parent);
	}

	public Field getField(String field)
	{
		return fields.stream()
			.filter(f -> f.name.equals(field))
			.findAny()
			.orElseThrow(() -> new RuntimeException("Unknown field " + field));
	}

//	@Override
//	public Set<Type> getUsedTypes()
//	{
//		return fields.stream()
//			.map(Field::getType)
//			.collect(Collectors.toSet());
//	}

	@Override
	public void visit(SyntaxVisitor visitor)
	{
		visitor.visitStruct(this);
	}

	//	@Override
//	public SourceBuilder toC()
//	{
//		return SourceBuilder.of("struct %s", name);
//	}
//
//	@Override
//	public SourceBuilder toPrototypeC()
//	{
//		return SourceBuilder.of("struct %s;", name);
//	}
//
//	@Override
//	public SourceBuilder toImplementationC()
//	{
//		SourceBuilder builder = new SourceBuilder();
//		builder.write("struct %s");
//		builder.scope();
//		for (Field field : fields)
//		{
//			builder.write("%s %s", field);
//		}
//		builder.unscope();
//		return builder;
//	}

	@Data
	@AllArgsConstructor
	public static class Field
	{
		private Struct struct;
		private String name;
		private Type type;

		public Variable access(Variable parent)
		{
			if (!parent.getType().equals(struct))
				throw new RuntimeException("Attempting to access field via incompatible variable");

			return Variable.builder()
				.name(name)
				.type(type)
				.parent(parent)
				.build();
		}
	}
}
