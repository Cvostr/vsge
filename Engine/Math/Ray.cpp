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

bool Ray::IntersectTriangle(
    const Vec3& v0,
    const Vec3& v1,
    const Vec3& v2, 
    float& distance,
    Vec2& position) const {
    Vec3 edge1 = v1 - v0;
    Vec3 edge2 = v2 - v0;
    Vec3 p = _direction.Cross(edge2);
    float det = edge1.Dot(p);
    Vec3 Perpendicular(0);

    if (det > std::numeric_limits<float>::epsilon())
    {
        Vec3 dist = _origin - v0;
        position.x = dist.Dot(p);
        if (position.x < 0.0f || position.x > det)
            return false;

        Perpendicular = dist.Cross(edge1);
        position.y = _direction.Dot(Perpendicular);
        if ((position.y < 0.f) || ((position.x + position.y) > det))
            return false;
    }
    else if (det < -std::numeric_limits<float>::epsilon())
    {
        Vec3 dist = _origin - v0;
        position.x = dist.Dot(p);
        if ((position.x > 0.f) || (position.x < det))
            return false;

        Perpendicular = dist.Cross(edge1);
        position.y = _direction.Dot(Perpendicular);
        if ((position.y > 0.0f) || (position.x + position.y < det))
            return false;
    }
    else
        return false;

    float inv_det = 1.f / det;
    distance = edge2.Dot(Perpendicular) * inv_det;
    position *= inv_det;

    return true;
}