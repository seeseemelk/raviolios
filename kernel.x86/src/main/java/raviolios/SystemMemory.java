package raviolios;

import raviolios.kit.core.Memory;

public class SystemMemory extends Memory
{
	@Override
	public int getTotalMemory()
	{
		return Arch.totalBytes();
	}

	@Override
	public int getUsedMemory()
	{
		return Arch.usedBytes();
	}

	@Override
	public int getFreeMemory()
	{
		return Arch.freeBytes();
	}
}
