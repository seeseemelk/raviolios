package be.seeseemelk.raviolios.plugin.cpp;

import lombok.Getter;
import org.gradle.api.file.ConfigurableFileCollection;
import org.gradle.api.model.ObjectFactory;
import org.gradle.api.provider.ListProperty;
import org.gradle.api.provider.Property;
import org.gradle.api.provider.SetProperty;
import org.gradle.api.tasks.InputFiles;

import javax.inject.Inject;

public abstract class CppExtension
{
	@InputFiles
	public abstract ConfigurableFileCollection getExtraSourceDirs();
	@InputFiles
	public abstract ConfigurableFileCollection getExtraHeaderDirs();
	public abstract ListProperty<String> getCommonFlags();
	public abstract ListProperty<String> getCppFlags();
	public abstract ListProperty<String> getLdFlags();
	public abstract ListProperty<String> getAsFlags();

	public abstract Property<String> getLinkerScript();
	public abstract Property<Boolean> getUseDocker();

	@Getter
	private final SetProperty<String> dependencies;

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
