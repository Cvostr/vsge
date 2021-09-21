using System;

class Entity : InternalObject {

    public void SetActive(bool active){

    }

    bool IsActive(){

    }

    public void SetName(string name){
        return i_SetName(handle, name);
    }

    public string GetName(){
        return i_GetName(handle);
    }

    public Entity GetParent(){

    }

    public void AddChild(Entity entity){

    }

    public void RemoveChild(Entity entity){

    }

    public Vec3 GetPosition(){
        return i_GetPosition(handle);
    }

    public void SetPosition(Vec3 position){

    }

    public Vec3 GetScale(){
        return i_GetScale(handle);
    }

    public void SetScale(Vec3 scale){

    }

    public Quat GetRotation(){
        return i_GetRotation(handle);
    }

    public void SetRotation(Quat rotation){

    }

    //INTERNALS

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern string i_GetName(ObjectHandle handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetName(ObjectHandle handle, string name);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern Vec3 i_GetPosition(ObjectHandle handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern Vec3 i_GetScale(ObjectHandle handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern Quat i_GetRotation(ObjectHandle handle);
}