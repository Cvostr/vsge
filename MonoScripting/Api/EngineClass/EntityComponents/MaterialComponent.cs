using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

class MaterialComponents : IEntityComponent {

    public MaterialComponents(ulong handle) : base(handle)
    {
    }

    public void SetMaterial(string resource_name){
        i_SetMaterial(handle, resource_name);
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern string i_SetMaterial(ulong handle, string material_name);

}