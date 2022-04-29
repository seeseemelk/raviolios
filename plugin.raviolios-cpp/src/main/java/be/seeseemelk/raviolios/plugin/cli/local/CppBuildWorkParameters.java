package be.seeseemelk.raviolios.plugin.cli.local;

import org.gradle.api.file.RegularFileProperty;
import org.gradle.api.provider.ListProperty;
import org.gradle.workers.WorkParameters;

public interface CppBuildWorkParameters extends WorkParameters
{
	ListProperty<String> getCommand();
	//RegularFileProperty getSource();
	RegularFileProperty getOutput();
}
