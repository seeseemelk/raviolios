package be.seeseemelk.raviolios.plugin.cpp.makefile;

import lombok.Getter;

import java.util.*;

public class RebuildTracker
{
	private final Map<String, Set<String>> rebuildMap = new HashMap<>();

	@Getter
	private final Map<String, Boolean> toRebuild = new HashMap<>();

	public RebuildTracker(MakefileDependencies dependencies)
	{
		for (Map.Entry<String, Set<String>> entry : dependencies.getDependencies().entrySet())
		{
			findCppFile(entry.getValue()).ifPresent(sourceFile -> {
				toRebuild.put(sourceFile, false);
				for (String dependency : entry.getValue())
				{
					rebuildMap.computeIfAbsent(dependency, k -> new HashSet<>()).add(sourceFile);
				}
			});
		}
	}

	private Optional<String> findCppFile(Set<String> value)
	{
		return value.stream()
			.filter(file -> file.endsWith(".cpp"))
			.findAny();
	}

	public void markAsChanged(String dependency)
	{
		for (String rebuild : rebuildMap.getOrDefault(dependency, Collections.emptySet()))
		{
			toRebuild.put(rebuild, true);
		}
	}

	public boolean needsRebuild(String file)
	{
		return toRebuild.getOrDefault(file, Boolean.TRUE);
	}
}
