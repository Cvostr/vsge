using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

enum MessageType{
    Info = 1,
    Warning = 2,
    Error = 3
}

class Logger{
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void Log(MessageType type, string message);
}