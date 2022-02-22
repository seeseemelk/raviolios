package be.seeseemelk.raviolios.plugin.cpp.makefile;

import lombok.Getter;
import lombok.NonNull;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

public class MakefileDependencies
{
	@Getter
	private Map<String, Set<String>> dependencies = new HashMap<>();

	public Set<String> getDependenciesFor(String file)
	{
		return dependencies.computeIfAbsent(file, key -> new HashSet<>());
	}

	public void addDependency(@NonNull String file, @NonNull String dependency)
	{
		getDependenciesFor(file).add(dependency);
	}
}
