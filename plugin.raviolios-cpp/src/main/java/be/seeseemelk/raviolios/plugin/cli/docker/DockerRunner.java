package be.seeseemelk.raviolios.plugin.cli.docker;

import be.seeseemelk.raviolios.plugin.cli.CommandRunner;
import com.github.dockerjava.api.DockerClient;
import com.github.dockerjava.core.DefaultDockerClientConfig;
import com.github.dockerjava.core.DockerClientConfig;
import com.github.dockerjava.core.DockerClientImpl;
import com.github.dockerjava.httpclient5.ApacheDockerHttpClient;
import com.github.dockerjava.transport.DockerHttpClient;

import java.time.Duration;

public class DockerRunner implements CommandRunner
{
	private DockerClient client;

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
		client.execCreateCmd("").
	}

	public void run()
	{
		client.startContainerCmd("");
	}
}
