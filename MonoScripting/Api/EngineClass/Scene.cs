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

    //public Entity[] GetAllEntitiesByName(string name){

    //} 

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern ulong i_GetRootNode(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern ulong i_GetEntity(ulong handle, string name);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern ulong i_AddNewEntity(ulong handle, string name);

}