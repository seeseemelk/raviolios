package raviolios.kit.core.devices.terminal;

import raviolios.kit.core.devices.AbstractDevice;

/**
 * A device that shows characters in a 2D grid, with a movable cursor.
 */
public abstract class AbstractTerminalDevice extends AbstractDevice
{
	/**
	 * Gets the number of characters that can be displayed horizontally.
	 *
	 * @return The width of the terminal.
	 */
	public abstract int getWidth();

	/**
	 * Gets the number of characters that can be displayed vertically.
	 *
	 * @return The height of the terminal.
	 */
	public abstract int getHeight();

	/**
	 * Places a character at a location.
	 * @param x The X position. 0 refers to the left side of the screen.
	 * @param y The Y position. 0 refers to the top side of the screen.
	 * @param character The character to place.
	 */
	public abstract void put(int x, int y, char character);

	/**
	 * Places a string at a location.
	 *
	 * @param x The X position. 0 refers to the left side of the screen.
	 * @param y The Y position. 0 refers to the top side of the screen.
	 * @param string The string to place.
	 */
	public void put(int x, int y, String string)
	{
		for (int i = 0; i < string.length(); i++)
		{
			put(x + i, y, string.charAt(i));
		}
	}
}
