using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public enum ResourceState {
	RESOURCE_STATE_UNLOADED = 0,
	RESOURCE_STATE_QUEUED,
	RESOURCE_STATE_LOADING,
	RESOURCE_STATE_LOADED,
	RESOURCE_STATE_READY,
	RESOURCE_STATE_LOADING_FAILED
};


public enum ResourceType {
	RESOURCE_TYPE_NONE = 0,
	RESOURCE_TYPE_TEXTURE,
	RESOURCE_TYPE_MESHGROUP,
	RESOURCE_TYPE_MESH,
	RESOURCE_TYPE_AUDIOCLIP,
	RESOURCE_TYPE_ANIMATION,
	RESOURCE_TYPE_MATERIAL,
	RESOURCE_TYPE_SCRIPT,
	RESOURCE_TYPE_SCENE,
	RESOURCE_TYPE_PREFAB
};

public class Resource : InternalObject {

    public Resource(ulong handle) : base(handle)
    {
    }

    public Resource(string name){
        handle = i_GetPointerByName(name);
    }

    public ResourceState GetState(){
        return i_GetState(handle);
    }

    public ResourceType GetResourceType(){
        return i_GetType(handle);
    }

    public string GetName(){
        return i_GetName(handle);
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern ulong i_GetPointerByName(string name);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern string i_GetName(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern ResourceState i_GetState(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern ResourceType i_GetType(ulong handle);
}