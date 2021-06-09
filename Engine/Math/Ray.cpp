#include "Ray.hpp"

using namespace VSGE;

Vec3 Ray::GetPoint(float dist) {
	return _origin + _direction * dist;
}

float Ray::GetHitdistance(const AABB& box) {
    // Check for ray origin being inside the box
    if (box.IsInside(_origin))
        return 0.0f;

    float hit_distance = std::numeric_limits<float>::infinity();


    // Check for intersecting in the X-direction
    if (_origin.x < box.GetMin().x && _direction.x > 0.0f)
    {
        float x = (box.GetMin().x - _origin.x) / _direction.x;
        if (x < hit_distance)
        {
            Vec3 point = _origin + _direction * x;
            if (point.y >= box.GetMin().y && point.y <= box.GetMax().y && point.z >= box.GetMin().z && point.z <= box.GetMax().z)
            {
                hit_distance = x;
            }
        }
    }
    if (_origin.x > box.GetMax().x && _direction.x < 0.0f)
    {
        float x = (box.GetMax().x - _origin.x) / _direction.x;
        if (x < hit_distance)
        {
            Vec3 point = _origin + _direction * x;
            if (point.y >= box.GetMin().y && point.y <= box.GetMax().y && point.z >= box.GetMin().z && point.z <= box.GetMax().z)
            {
                hit_distance = x;
            }
        }
    }

    // Check for intersecting in the Y-direction
    if (_origin.y < box.GetMin().y && _direction.y > 0.0f)
    {
        float x = (box.GetMin().y - _origin.y) / _direction.y;
        if (x < hit_distance)
        {
            Vec3 point = _origin + _direction * x;
            if (point.x >= box.GetMin().x && point.x <= box.GetMax().x && point.z >= box.GetMin().z && point.z <= box.GetMax().z)
            {
                hit_distance = x;
            }
        }
    }
    if (_origin.y > box.GetMax().y && _direction.y < 0.0f)
    {
        float x = (box.GetMax().y - _origin.y) / _direction.y;
        if (x < hit_distance)
        {
            Vec3 point = _origin + _direction * x;
            if (point.x >= box.GetMin().x && point.x <= box.GetMax().x && point.z >= box.GetMin().z && point.z <= box.GetMax().z)
            {
                hit_distance = x;
            }
        }
    }

    // Check for intersecting in the Z-direction
    if (_origin.z < box.GetMin().z && _direction.z > 0.0f)
    {
        float x = (box.GetMin().z - _origin.z) / _direction.z;
        if (x < hit_distance)
        {
            Vec3 point = _origin + _direction * x;
            if (point.x >= box.GetMin().x && point.x <= box.GetMax().x && point.y >= box.GetMin().y && point.y <= box.GetMax().y)
            {
                hit_distance = x;
            }
        }
    }
    if (_origin.z > box.GetMax().z && _direction.z < 0.0f)
    {
        float x = (box.GetMax().z - _origin.z) / _direction.z;
        if (x < hit_distance)
        {
            Vec3 point = _origin + _direction * x;
            if (point.x >= box.GetMin().x && point.x <= box.GetMax().x && point.y >= box.GetMin().y && point.y <= box.GetMax().y)
            {
                hit_distance = x;
            }
        }
    }

    return hit_distance;
}