using System;

public abstract class EntityScript{
    private ulong entity_ptr = 0;
    public Entity GetEntity(){
        return new Entity(entity_ptr);
    }
}