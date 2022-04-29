package be.seeseemelk.raviolios.plugin.cli;

public interface CommandRunner extends AutoCloseable
{
    void run(Arguments command);

    void await();
}
