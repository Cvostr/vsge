using System;
using System.Runtime.CompilerServices;

public class Material : InternalObject {

    public Material(ulong handle) : base(handle)
    {
        
    }

    public void SetValue<T>(string key, T value){
        if(typeof(T).Equals(typeof(int))){
            i_SetIntValue(handle, key, (int)(object)value);
        }
        if(typeof(T).Equals(typeof(float))){
            i_SetFloatValue(handle, key, (float)(object)value);
        }
        if(typeof(T).Equals(typeof(Color))){
            i_SetColorValue(handle, key, (Color)(object)value);
        }
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern string i_SetIntValue(ulong handle, string key, int value);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern string i_SetFloatValue(ulong handle, string key, float value);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern string i_SetColorValue(ulong handle, string key, Color value);
}