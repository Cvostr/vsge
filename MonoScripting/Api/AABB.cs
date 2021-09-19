using System;

public struct AABB{
    private Vec3 _min;
    private Vec3 _max;

    public void Extend(AABB box){
        if (box._min.x < _min.x)
		    _min.x = box._min.x;
        if (box._min.y < _min.y)
            _min.y = box._min.y;
        if (box._min.z < _min.z)
            _min.z = box._min.z;

        if (box._max.x > _max.x)
            _max.x = box._max.x;
        if (box._max.y > _max.y)
            _max.y = box._max.y;
        if (box._max.z > _max.z)
            _max.z = box._max.z;
    }

    public void Extend(Vec3 point){
    	if (_min.x > point.x) _min.x = point.x;
	    if (_min.y > point.y) _min.y = point.y;
	    if (_min.z > point.z) _min.z = point.z;
				 			 
	    if (_max.x < point.x) _max.x = point.x;
	    if (_max.y < point.y) _max.y = point.y;
	    if (_max.z < point.z) _max.z = point.z;
    }

    public Vec3 GetCenter(){
        return (_min + _max) * 0.5f;
    }

    public Vec3 GetSize(){
        return (_max - _min) * 0.5f;
    }

}