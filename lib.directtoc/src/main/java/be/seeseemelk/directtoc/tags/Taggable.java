package be.seeseemelk.directtoc.tags;

public interface Taggable
{
	<T extends Tag> void addTag(T tag);
	<T extends Tag> boolean hasTag(Class<T> tagType);
	default <T extends Tag> boolean hasTag(T tag)
	{
		return hasTag(tag.getClass());
	}
	<T extends Tag> T getTag(Class<T> tagType);
}
