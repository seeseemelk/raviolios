package be.seeseemelk.directtoc.tags;

import lombok.EqualsAndHashCode;

import java.util.HashMap;
import java.util.Map;

@EqualsAndHashCode
public class Tags implements Taggable
{
	private final Map<Class<? extends Tag>, Tag> tags = new HashMap<>();

	@Override
	public <T extends Tag> void addTag(T tag)
	{
		tags.put(tag.getClass(), tag);
	}

	@Override
	public <T extends Tag> boolean hasTag(Class<T> tagType)
	{
		return tags.containsKey(tagType);
	}



	@SuppressWarnings("unchecked")
	@Override
	public <T extends Tag> T getTag(Class<T> tagType)
	{
		return (T) tags.get(tagType);
	}
}
