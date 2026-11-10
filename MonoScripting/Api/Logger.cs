using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public enum MessageType {
    Info = 1,
    Warning = 2,
    Error = 3
}

public class Logger
{
    public static void Log(string message)
    {
        Log(MessageType.Info, message);
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void Log(MessageType type, string message);
}