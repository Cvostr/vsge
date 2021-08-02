#pragma once

#include "Mat4.hpp"
#include "Vec3.hpp"
#include "Quaternion.hpp"

Mat4 GetTranslationMatrix(const Vec3& pos);
Mat4 GetScaleMatrix(const Vec3& scale);

Mat4 GetRotationEuler(const Mat4& m, float angle, const Vec3& v);
Mat4 GetRotationXMatrixEuler(float angle);
Mat4 GetRotationYMatrixEuler(float angle);
Mat4 GetRotationZMatrixEuler(float angle);

Mat4 RemoveRotationFromTransform(const Mat4& transform, const Mat4& camera_view);

Mat4 GetRotationMatrix(const Vec3 Rotation);
Mat4 GetRotationMatrix(const Quat Rotation);

Mat4 GetTransform(const Vec3& pos, const Vec3& scale, const Vec3& rotation);
Mat4 GetTransform(const Vec3& pos, const Vec3& scale, const Quat& rotation);

Mat4 Translate(Mat4& origin, const Vec3& translation);
Mat4 Scale(Mat4& origin, const Vec3& scale);