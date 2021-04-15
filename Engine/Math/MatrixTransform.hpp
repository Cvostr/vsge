#pragma once

#include "Mat4.hpp"
#include "Vec3.hpp"

Mat4 GetTranslationMatrix(const Vec3& pos);
Mat4 GetScaleMatrix(const Vec3& scale);

Mat4 GetRotationXMatrixEuler(float angle);
Mat4 GetRotationYMatrixEuler(float angle);
Mat4 GetRotationZMatrixEuler(float angle);