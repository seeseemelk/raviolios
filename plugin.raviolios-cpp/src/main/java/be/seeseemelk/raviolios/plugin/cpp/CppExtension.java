package be.seeseemelk.raviolios.plugin.cpp;

import lombok.Getter;
import lombok.Setter;
import org.gradle.api.file.RegularFileProperty;
import org.gradle.api.file.SourceDirectorySet;
import org.gradle.api.model.ObjectFactory;
import org.gradle.api.provider.ListProperty;
import org.gradle.api.provider.Property;
import org.gradle.api.provider.SetProperty;

import javax.inject.Inject;
import java.util.HashSet;
import java.util.Set;

public abstract class CppExtension
{
	public abstract ListProperty<String> getCommonFlags();
	public abstract ListProperty<String> getCppFlags();
	public abstract ListProperty<String> getLdFlags();
	public abstract ListProperty<String> getAsFlags();

	public abstract RegularFileProperty getLinkerScript();

	@Getter
	private SetProperty<String> dependencies;

	@Inject
	public CppExtension(ObjectFactory objectFactory)
	{
		dependencies = objectFactory.setProperty(String.class);
	}

	public void dependency(String name)
	{
		dependencies.add(name);
	}
}
