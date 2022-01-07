using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class Scenes{
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void AddScene();

    public static Scene GetMainScene(){
        return new Scene(i_GetMainScene());
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern ulong i_GetMainScene();

    public static Scene[] GetScenes(){
        ulong[] pointers = i_GetScenes();
        Scene[] result = new Scene[pointers.Length];

        for(uint i = 0; i < result.Length; i ++){
            result[i] = new Scene(pointers[i]);
        }

        return result;
    } 

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern ulong[] i_GetScenes();

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void LoadScene(string resource_name, uint scene_slot);
}