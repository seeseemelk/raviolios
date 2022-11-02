package be.seeseemelk.directtoc.visitors.interpreter;

import lombok.AllArgsConstructor;

@AllArgsConstructor
public class IntValue extends Value
{
	private Integer integer;

	@Override
	public int asInt()
	{
		return integer;
	}
}
