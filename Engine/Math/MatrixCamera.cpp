#include "MatrixCamera.hpp"
#include <cmath>
#include "MathBase.hpp"

Mat4 GetPerspectiveRH_Default(PERSPECTIVE_ARGS) {
	Mat4 Result(0);
	float _fov = tan(to_radians(fov) / 2.f);

	Result.Values[0][0] = 1.f / (aspect * _fov);
	Result.Values[1][1] = 1.f / _fov;
	Result.Values[2][2] = -(far + near) / (far - near);
	Result.Values[2][3] = -1.f;
	Result.Values[3][2] = -(2.f * far * near) / (far - near);

	return Result;
}
Mat4 GetPerspectiveLH_Default(PERSPECTIVE_ARGS) {
	Mat4 Result(0);
	float _fov = tan(to_radians(fov) / 2.f);

	Result.Values[0][0] = 1.f / (aspect * _fov);
	Result.Values[1][1] = 1.f / _fov;
	Result.Values[2][2] = (far + near) / (far - near);
	Result.Values[2][3] = 1.f;
	Result.Values[3][2] = -(2.f * far * near) / (far - near);

	return Result;
}

Mat4 GetPerspectiveRH_ZeroOne(PERSPECTIVE_ARGS) {
	Mat4 Result(0);
	float _fov = tan(to_radians(fov) / 2.f);

	Result.Values[0][0] = 1.f / (aspect * _fov);
	Result.Values[1][1] = -1.f / (_fov);
	Result.Values[2][2] = far / (near - far);
	Result.Values[2][3] = -1.f;
	Result.Values[3][2] = -(far * near) / (far - near);

	return Result;
}
Mat4 GetPerspectiveLH_ZeroOne(PERSPECTIVE_ARGS) {
	Mat4 Result(0);
	float _fov = tan(to_radians(fov) / 2.f);

	Result.Values[0][0] = 1.f / (aspect * _fov);
	Result.Values[1][1] = 1.f / (_fov);
	Result.Values[2][2] = far / (far - near);
	Result.Values[2][3] = 1.f;
	Result.Values[3][2] = -(far * near) / (far - near);

	return Result;
}

Mat4 GetOrthoLH_ZeroOne(ORTHO_ARGS) {
	Mat4 Result(1);
	Result.Values[0][0] = 2.f / (right - left);
	Result.Values[1][1] = 2.f / (top - bottom);
	Result.Values[2][2] = 1.f / (far - near);
	Result.Values[3][0] = -(right + left) / (right - left);
	Result.Values[3][1] = -(top + bottom) / (top - bottom);
	Result.Values[3][2] = -near / (far - near);
	return Result;
}
Mat4 GetOrthoRH_ZeroOne(ORTHO_ARGS) {
	Mat4 Result(1);

	Result.Values[0][0] = 2.f / (right - left);
	Result.Values[1][1] = 2.f / (top - bottom);
	Result.Values[2][2] = -1.f / (far - near);
	Result.Values[3][0] = -(right + left) / (right - left);
	Result.Values[3][1] = -(top + bottom) / (top - bottom);
	Result.Values[3][2] = -near / (far - near);

	return Result;
}
Mat4 GetOrthoLH_Default(ORTHO_ARGS) {
	Mat4 Result(1);

	Result.Values[0][0] = 2.f / (right - left);
	Result.Values[1][1] = 2.f / (top - bottom);
	Result.Values[2][2] = 2.f / (far - near);
	Result.Values[3][0] = -(right + left) / (right - left);
	Result.Values[3][1] = -(top + bottom) / (top - bottom);
	Result.Values[3][2] = -(far + near) / (far - near);

	return Result;
}
Mat4 GetOrthoRH_Default(ORTHO_ARGS) {
	Mat4 Result(1);

	Result.Values[0][0] = 2.f / (right - left);
	Result.Values[1][1] = 2.f / (top - bottom);
	Result.Values[2][2] = -2.f / (far - near);
	Result.Values[3][0] = -(right + left) / (right - left);
	Result.Values[3][1] = -(top + bottom) / (top - bottom);
	Result.Values[3][2] = -(far + near) / (far - near);

	return Result;
}

Mat4 GetViewRH(const Vec3& eye, const Vec3& center, const Vec3& up) {
	Vec3 f = (center - eye).GetNormalized();
	Vec3 s = f.Cross(up).GetNormalized();
	Vec3 u = s.Cross(f);

	Mat4 Result(1);
	Result.Values[0][0] = s.x;
	Result.Values[1][0] = s.y;
	Result.Values[2][0] = s.z;
	Result.Values[0][1] = u.x;
	Result.Values[1][1] = u.y;
	Result.Values[2][1] = u.z;
	Result.Values[0][2] = -f.x;
	Result.Values[1][2] = -f.y;
	Result.Values[2][2] = -f.z;
	Result.Values[3][0] = -s.Dot(eye);
	Result.Values[3][1] = -u.Dot(eye);
	Result.Values[3][2] = f.Dot(eye);
	return Result;
}
Mat4 GetViewLH(const Vec3& center, const Vec3& eye, const Vec3& up) {
	Vec3 f = (center - eye).GetNormalized();
	Vec3 s = up.Cross(f).GetNormalized();
	Vec3 u = f.Cross(s);

	Mat4 Result(1);
	Result.Values[0][0] = s.x;
	Result.Values[1][0] = s.y;
	Result.Values[2][0] = s.z;
	Result.Values[0][1] = u.x;
	Result.Values[1][1] = u.y;
	Result.Values[2][1] = u.z;
	Result.Values[0][2] = f.x;
	Result.Values[1][2] = f.y;
	Result.Values[2][2] = f.z;
	Result.Values[3][0] = -s.Dot(eye);
	Result.Values[3][1] = -u.Dot(eye);
	Result.Values[3][2] = -f.Dot(eye);
	return Result;
}