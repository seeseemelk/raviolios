package be.seeseemelk.raviolios.plugin.cli.docker;

import be.seeseemelk.raviolios.plugin.cli.Arguments;
import be.seeseemelk.raviolios.plugin.cli.CommandRunner;
import com.github.dockerjava.api.DockerClient;
import com.github.dockerjava.api.command.CreateContainerResponse;
import com.github.dockerjava.api.model.Bind;
import com.github.dockerjava.api.model.HostConfig;
import com.github.dockerjava.core.DefaultDockerClientConfig;
import com.github.dockerjava.core.DockerClientConfig;
import com.github.dockerjava.core.DockerClientImpl;
import com.github.dockerjava.httpclient5.ApacheDockerHttpClient;
import com.github.dockerjava.transport.DockerHttpClient;

import java.time.Duration;

public class DockerRunner implements CommandRunner
{
	private final DockerClient client;
	private String containerId = null;

	public DockerRunner()
	{
		DockerClientConfig config = DefaultDockerClientConfig.createDefaultConfigBuilder().build();

		DockerHttpClient httpClient = new ApacheDockerHttpClient.Builder()
			.dockerHost(config.getDockerHost())
			.sslConfig(config.getSSLConfig())
			.maxConnections(100)
			.connectionTimeout(Duration.ofSeconds(30))
			.responseTimeout(Duration.ofSeconds(45))
			.build();

		client = DockerClientImpl.getInstance(config, httpClient);
	}

	public void start()
	{
		CreateContainerResponse response = client.createContainerCmd("seeseemelk/raviolios-build-light")
				.withHostConfig(HostConfig.newHostConfig()
						.withBinds(Bind.parse("/home/seeseemelk/dev/raviolios:/project")))
				.exec();
		containerId = response.getId();
		client.startContainerCmd(containerId).exec();
	}

	@Override
	public void close() throws Exception
	{
		try
		{
			client.stopContainerCmd(containerId).exec();
		}
		finally
		{
			client.removeContainerCmd(containerId).exec();
		}
	}

	@Override
	public void run(Arguments command)
	{
		//client.startContainerCmd("");
	}

	@Override
	public void await()
	{

	}
}
