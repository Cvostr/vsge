#pragma once

#include "Mat4.hpp"
#include "Vec3.hpp"
#include "Quaternion.hpp"

Mat4 GetTranslationMatrix(const Vec3& pos);
Mat4 GetScaleMatrix(const Vec3& scale);

Mat4 GetRotationXMatrixEuler(float angle);
Mat4 GetRotationYMatrixEuler(float angle);
Mat4 GetRotationZMatrixEuler(float angle);

Mat4 GetRotationMatrix(const Vec3 Rotation);
Mat4 GetRotationMatrix(const Quat Rotation);

Mat4 GetTransform(const Vec3& pos, const Vec3& scale, const Vec3& rotation);