#include "Frustum.hpp"

using namespace VSGE;

void Frustum::Update(const Mat4& PV) {
    // Calculate near plane of frustum.
    Vec3 normal = Vec3(PV.M14 + PV.M13, PV.M24 + PV.M23, PV.M34 + PV.M33);
    _planes[0].SetNormal(normal);
    _planes[0].SetD(PV.M44 + PV.M43);
    _planes[0].Normalize();

    // Calculate far plane of frustum.
    normal = Vec3(PV.M14 - PV.M13, PV.M24 - PV.M23, PV.M34 - PV.M33);
    _planes[1].SetNormal(normal);
    _planes[1].SetD(PV.M44 - PV.M43);
    _planes[1].Normalize();

    // Calculate left plane of frustum.
    normal = Vec3(PV.M14 + PV.M11, PV.M24 + PV.M21, PV.M34 + PV.M31);
    _planes[2].SetNormal(normal);
    _planes[2].SetD(PV.M44 + PV.M41);
    _planes[2].Normalize();

    // Calculate right plane of frustum.
    normal = Vec3(PV.M14 - PV.M11, PV.M24 - PV.M21, PV.M34 - PV.M31);
    _planes[3].SetNormal(normal);
    _planes[3].SetD(PV.M44 - PV.M41);
    _planes[3].Normalize();

    // Calculate top plane of frustum.
    normal = Vec3(PV.M14 - PV.M12, PV.M24 - PV.M22, PV.M34 - PV.M32);
    _planes[4].SetNormal(normal);
    _planes[4].SetD(PV.M44 - PV.M42);
    _planes[4].Normalize();

    // Calculate bottom plane of frustum.
    normal = Vec3(PV.M14 + PV.M12, PV.M24 + PV.M22, PV.M34 + PV.M32);
    _planes[5].SetNormal(normal);
    _planes[5].SetD(PV.M44 + PV.M42);
    _planes[5].Normalize();
}

FrustumIntersection Frustum::SphereInside(const Vec3& center, float radius) {
    // calculate our distances to each of the planes
    for (auto& plane : _planes)
    {
        // find the distance to this plane
        float distance = plane.GetNormal().Dot(center) + plane.GetD();

        // if this distance is < -sphere.radius, we are outside
        if (distance < -radius)
            return FRUSTUM_OUTSIDE;

        // else if the distance is between +- radius, then we intersect
        if (fabs(distance) < radius)
            return FRUSTUM_INTERSECTS;
    }

    // otherwise we are fully in view
    return FRUSTUM_INSIDE;
}

FrustumIntersection Frustum::CheckAABB(const AABB& aabb) {
    Vec3 center = aabb.GetCenter();
    Vec3 edge = center - aabb.GetMin();
    bool allInside = true;

    for (unsigned int plane_i = 0; plane_i < 6; plane_i++)
    {
        Plane* plane_ptr = &_planes[plane_i];
        Vec3 PlaneNormal = plane_ptr->GetNormal();
        Vec3 PlaneNormalAbs = Vec3(fabs(PlaneNormal.x), fabs(PlaneNormal.y), fabs(PlaneNormal.z));
        PlaneNormalAbs = PlaneNormalAbs.GetNormalized();

        float dist = PlaneNormal.Dot(center) + plane_ptr->GetD();
        float absDist = PlaneNormalAbs.Dot(edge);

        if (dist < -absDist)
            return FRUSTUM_OUTSIDE;
        else if (dist < absDist)
            allInside = false;
    }

    return allInside ? FRUSTUM_INSIDE : FRUSTUM_INTERSECTS;
}