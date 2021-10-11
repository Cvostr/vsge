using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class IEntityComponent : InternalObject {

    public IEntityComponent(ulong handle) : base(handle)
    {
    }

    public IEntityComponent()
    {
    }

    Entity GetEntity(){
        return new Entity(i_GetEntity(handle));
    }

    bool IsActive(){
        return i_IsActive(handle);
    }

    void SetActive(bool active){
        i_SetActive(handle, active);
    }

    int GetComponentType(){
        return i_GetType(handle);
    }

    public static int GetComponentID<T>(){
        if(typeof(T).Equals(typeof(LightSourceComponent)))
            return 1;
        if(typeof(T).Equals(typeof(MaterialComponent)))
            return 2;
        if(typeof(T).Equals(typeof(MeshComponent)))
            return 3;
        if(typeof(T).Equals(typeof(AnimatorComponent)))
            return 4;
        if(typeof(T).Equals(typeof(AudioSourceComponent)))
            return 6;
        if(typeof(T).Equals(typeof(CameraComponent)))
            return 7;
        if(typeof(T).Equals(typeof(RigidBodyComponent)))
            return 8;
        if(typeof(T).Equals(typeof(ColliderComponent)))
            return 9;
        
        return -1;
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern ulong i_GetEntity(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern bool i_IsActive(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetActive(ulong handle, bool active);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern int i_GetType(ulong handle);

}