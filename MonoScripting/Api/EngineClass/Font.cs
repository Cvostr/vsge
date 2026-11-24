using System;
using System.Runtime.CompilerServices;

public class Font : InternalObject
{
	public Font(ulong handle) : base(handle)
	{

	}

	public Vec2i GetSizeOfString(string text)
	{
		i_GetSizeOfString(handle, text, out Vec2i result);
		return result;
	}

	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern void i_GetSizeOfString(ulong handle, string text, out Vec2i size);
}