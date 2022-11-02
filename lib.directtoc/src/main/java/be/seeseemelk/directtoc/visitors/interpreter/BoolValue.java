package be.seeseemelk.directtoc.visitors.interpreter;

import lombok.RequiredArgsConstructor;

@RequiredArgsConstructor
public class BoolValue extends Value
{
	public static BoolValue TRUE = new BoolValue(true);
	public static BoolValue FALSE = new BoolValue(false);

	private final Boolean bool;

	@Override
	public boolean asBool()
	{
		return bool;
	}
}
