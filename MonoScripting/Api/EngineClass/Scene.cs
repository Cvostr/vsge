using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class Scene : InternalObject {

    public Scene(ulong handle) : base(handle)
    {
    }

    public Entity GetRootEntity(){
        return new Entity(i_GetRootNode(handle));
    }

    public Entity AddNewEntity(string name){
        return new Entity(i_AddNewEntity(handle, name));
    }

    public Entity GetEntityByName(string name){
        return new Entity(i_GetEntity(handle, name));
    }

    public Entity[] GetAllEntitiesByName(string name){
        ulong[] pointers = i_GetAllEntitiesByName(handle, name);
        Entity[] result = new Entity[pointers.Length];

        for(uint i = 0; i < result.Length; i ++){
            result[i] = new Entity(pointers[i]);
        }

        return result;
    } 

    public void Run(){
        i_Run(handle);
    }

    public void Pause(){
        i_Pause(handle);
    }

    public void Stop(){
        i_Stop(handle);
    }

    bool IsRunning(){
        return i_IsRunning(handle);
    }

    bool IsPaused(){
        return i_IsPaused(handle);
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern ulong i_GetRootNode(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern ulong i_GetEntity(ulong handle, string name);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern ulong i_AddNewEntity(ulong handle, string name);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern ulong[] i_GetAllEntitiesByName(ulong handle, string name);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_Run(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_Pause(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_Stop(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern bool i_IsRunning(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern bool i_IsPaused(ulong handle);

}