using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class MaterialComponent : IEntityComponent {

    public MaterialComponent(ulong handle) : base(handle)
    {
    }

    public void SetMaterial(string resource_name){
        i_SetMaterial(handle, resource_name);
    }

    void SetMaterial(Resource resource){
        i_SetMaterial(handle, resource.GetHandle());
    }

    bool IsCastShadows(){
        return i_IsCastShadows(handle);
    }

    void SetCastShadows(bool cast){
        i_SetCastShadows(handle, cast);
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern string i_SetMaterial(ulong handle, string material_name);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern string i_SetMaterial(ulong handle, ulong resource_handle);


    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern bool i_IsCastShadows(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetCastShadows(ulong handle, bool cast);
}