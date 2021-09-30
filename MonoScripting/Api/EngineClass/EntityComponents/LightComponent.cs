using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public enum LightType {
	LIGHT_TYPE_DIRECTIONAL,
	LIGHT_TYPE_POINT,
	LIGHT_TYPE_SPOT
};

public class LightSourceComponent : IEntityComponent {

    public LightSourceComponent(ulong handle) : base(handle)
    {
    }

    public LightType GetLightType(){
        return i_GetType(handle);
    }

    void SetLightType(LightType type){
        i_SetType(handle, type);
    }

    Color GetColor(){
        return i_GetColor(handle);
    }

    void SetColor(Color color){
        i_SetColor(handle, color);
    }

    public float GetIntensity(){
        return i_GetIntensity(handle);
    }

    public void SetIntensity(float intensity){
        i_SetIntensity(handle, intensity);
    }

    public float GetRange(){
        return i_GetRange(handle);
    }

    public void SetRange(float range){
        i_SetRange(handle, range);
    }

    public float GetSpotAngle(){
        return i_GetSpotAngle(handle);
    }

    public void SetSpotAngle(float range){
        i_SetSpotAngle(handle, range);
    }

    Vec3 GetDirection(){
        return i_GetDirection(handle);
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern LightType i_GetType(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetType(ulong handle, LightType type);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern Color i_GetColor(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetColor(ulong handle, Color color);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern float i_GetIntensity(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetIntensity(ulong handle, float intensity);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern float i_GetRange(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetRange(ulong handle, float range);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern float i_GetSpotAngle(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetSpotAngle(ulong handle, float angle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern Vec3 i_GetDirection(ulong handle);
}