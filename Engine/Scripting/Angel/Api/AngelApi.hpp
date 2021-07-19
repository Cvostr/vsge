#pragma once

#define VEC3_TYPE_NAME "Vec3"
#define QUAT_TYPE_NAME "Quat"

#define SCENE_TYPE_NAME "Scene"
#define ENTITY_TYPE_NAME "Entity"

namespace VSGE {
	void BindMath();
	void BindIO();
	void BindScene();

    template <typename T>
    static void new_as_obj_T(T* ptr) {
        new (ptr) T();
    }
    template <typename T>
    static void del_as_obj_T(T* ptr) {
        ((T*)ptr)->~T();
    }
    template <typename T>
    static T* new_as_ref_T() {
        return new T;
    }
    template <typename T, typename U>
    static U* ag_cast(T* ptr) {
        return static_cast<U*>(ptr);
    }
}