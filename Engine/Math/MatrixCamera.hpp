#pragma once

#include "Mat4.hpp"
#include "Vec3.hpp"

#define PERSPECTIVE_ARGS float fov, float aspect, float near, float far
#define ORTHO_ARGS float left, float right, float bottom, float top, float far, float near
#define VIEW_ARGS const Vec3& center, const Vec3& eye, const Vec3& up

Mat4 GetPerspectiveRH_ZeroOne(PERSPECTIVE_ARGS);
Mat4 GetPerspectiveLH_ZeroOne(PERSPECTIVE_ARGS);
Mat4 GetPerspectiveRH_Default(PERSPECTIVE_ARGS);
Mat4 GetPerspectiveLH_Default(PERSPECTIVE_ARGS);

Mat4 GetOrthoLH_ZeroOne(ORTHO_ARGS);
Mat4 GetOrthoRH_ZeroOne(ORTHO_ARGS);
Mat4 GetOrthoLH_Default(ORTHO_ARGS);
Mat4 GetOrthoRH_Default(ORTHO_ARGS);

Mat4 GetViewRH(VIEW_ARGS);
Mat4 GetViewLH(VIEW_ARGS);