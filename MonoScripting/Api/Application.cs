using System;
using System.Runtime.CompilerServices;

class Application{
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void Quit();

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern bool IsHeadless();

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern bool IsServer();

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern string GetAppDirectoryPath();
}