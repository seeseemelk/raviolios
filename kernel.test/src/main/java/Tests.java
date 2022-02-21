public class Tests
{

	public static int returnInt()
	{
		return 4;
	}

	public static int callFunction()
	{
		return returnInt();
	}

	public static native void nativeTest();
	public static void callNative()
	{
		nativeTest();
	}
}