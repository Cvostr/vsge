using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public enum ResourceState {
	Unloaded = 0,
	Queued,
	Loading,
	Loaded,
	Ready,
	LoadingFailed
};


public enum ResourceType {
	None = 0,
	Texture,
	MeshGroup,
	Mesh,
	AudioClip,
	Animation,
	Material,
	Script,
	Scene,
	Prefab
};

public class Resource : InternalObject {

    public Resource(ulong handle) : base(handle)
    {
    }

    public ResourceState GetState()
    {
        return i_GetState(handle);
    }

    public ResourceType GetResourceType()
    {
        return i_GetType(handle);
    }

    public string GetName()
    {
        return i_GetName(handle);
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern string i_GetName(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern ResourceState i_GetState(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern ResourceType i_GetType(ulong handle);
}