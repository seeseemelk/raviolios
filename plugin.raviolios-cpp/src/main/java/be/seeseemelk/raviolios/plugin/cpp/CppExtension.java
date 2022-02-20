package be.seeseemelk.raviolios.plugin.cpp;

import org.gradle.api.Project;
import org.gradle.api.provider.Property;

import java.util.HashSet;
import java.util.Set;

public abstract class CppExtension
{
	public abstract Property<Project> getDependencies();

	/*
	public Set<String> getDependencies()
	{
		return dependencies;
	}

	public void setDependencies(Set<String> dependencies)
	{
		this.dependencies = dependencies;
	}*/

//	public void dependency(Project dependency)
//	{
//		getDependencies().get().add(dependency);
//	}

	//	public void dependency(String dep)
//	{
//		getDependencies().add(dep);
//	}
}
