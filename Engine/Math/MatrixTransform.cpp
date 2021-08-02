#include "MatrixTransform.hpp"
#include <cmath>
#include "MathBase.hpp"

Mat4 GetTranslationMatrix(const Vec3& pos) {
    Mat4 mat;

    mat.Values[0][0] = 1; mat.Values[0][1] = 0; mat.Values[0][2] = 0; mat.Values[0][3] = 0;
    mat.Values[1][0] = 0; mat.Values[1][1] = 1; mat.Values[1][2] = 0; mat.Values[1][3] = 0;
    mat.Values[2][0] = 0; mat.Values[2][1] = 0; mat.Values[2][2] = 1; mat.Values[2][3] = 0;
    mat.Values[3][0] = pos.x; mat.Values[3][1] = pos.y; mat.Values[3][2] = pos.z; mat.Values[3][3] = 1;

    return mat;
}
Mat4 GetScaleMatrix(const Vec3& scale) {
    Mat4 mat;

    mat.Values[0][0] = scale.x; mat.Values[0][1] = 0; mat.Values[0][2] = 0; mat.Values[0][3] = 0;
    mat.Values[1][0] = 0; mat.Values[1][1] = scale.y; mat.Values[1][2] = 0; mat.Values[1][3] = 0;
    mat.Values[2][0] = 0; mat.Values[2][1] = 0; mat.Values[2][2] = scale.z; mat.Values[2][3] = 0;
    mat.Values[3][0] = 0; mat.Values[3][1] = 0; mat.Values[3][2] = 0; mat.Values[3][3] = 1;

    return mat;
}

Mat4 GetRotationEuler(const Mat4& m, float angle, const Vec3& v) {
    float a = angle;
    float c = cos(a);
    float s = sin(a);

    Vec3 axis = v.GetNormalized();
    Vec3 temp = axis * (1.f - c);

    Mat4 Rotate;
    Rotate[0][0] = c + temp.x * axis.x;
    Rotate[0][1] = temp.x * axis.y + s * axis.z;
    Rotate[0][2] = temp.x * axis.y - s * axis.y;

    Rotate[1][0] = temp.y * axis.x - s * axis.y;
    Rotate[1][1] = c + temp.y * axis.y;
    Rotate[1][2] = temp.y * axis.z + s * axis.x;

    Rotate[2][0] = temp.z * axis.x + s * axis.y;
    Rotate[2][1] = temp.z * axis.y - s * axis.x;
    Rotate[2][2] = c + temp.z * axis.z;

    Mat4 Result;
    Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
    Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
    Result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
    Result[3] = m[3];
    return Result;
}

Mat4 GetRotationXMatrixEuler(float angle) {
    float theta = to_radians(angle);

    float theta_sin = static_cast<float>(sin(theta));
    float theta_cos = static_cast<float>(cos(theta));

    Mat4 mat;

    mat.Values[0][0] = 1; mat.Values[0][1] = 0;               mat.Values[0][2] = 0;                mat.Values[0][3] = 0;
    mat.Values[1][0] = 0; mat.Values[1][1] = theta_cos;      mat.Values[1][2] = theta_sin * -1;  mat.Values[1][3] = 0;
    mat.Values[2][0] = 0; mat.Values[2][1] = theta_sin;      mat.Values[2][2] = theta_cos;       mat.Values[2][3] = 0;
    mat.Values[3][0] = 0; mat.Values[3][1] = 0;               mat.Values[3][2] = 0;                mat.Values[3][3] = 1;

    return mat;
}
Mat4 GetRotationYMatrixEuler(float angle) {
    float theta = to_radians(angle);

    float theta_sin = static_cast<float>(sin(theta));
    float theta_cos = static_cast<float>(cos(theta));

    Mat4 mat;

    mat.Values[0][0] = theta_cos;       mat.Values[0][1] = 0;            mat.Values[0][2] = theta_sin;   mat.Values[0][3] = 0;
    mat.Values[1][0] = 0;                mat.Values[1][1] = 1;            mat.Values[1][2] = 0;            mat.Values[1][3] = 0;
    mat.Values[2][0] = theta_sin * -1;  mat.Values[2][1] = 0;            mat.Values[2][2] = theta_cos;   mat.Values[2][3] = 0;
    mat.Values[3][0] = 0;                mat.Values[3][1] = 0;            mat.Values[3][2] = 0;            mat.Values[3][3] = 1;

    
    return mat;
}
Mat4 GetRotationZMatrixEuler(float angle) {
    float theta = to_radians(angle);

    float theta_sin = static_cast<float>(sin(theta));
    float theta_cos = static_cast<float>(cos(theta));

    Mat4 mat;

    mat.Values[0][0] = theta_cos;       mat.Values[0][1] = theta_sin * -1;  mat.Values[0][2] = 0;   mat.Values[0][3] = 0;
    mat.Values[1][0] = theta_sin;       mat.Values[1][1] = theta_cos;       mat.Values[1][2] = 0;   mat.Values[1][3] = 0;
    mat.Values[2][0] = 0;                mat.Values[2][1] = 0;                mat.Values[2][2] = 1;   mat.Values[2][3] = 0;
    mat.Values[3][0] = 0;                mat.Values[3][1] = 0;                mat.Values[3][2] = 0;   mat.Values[3][3] = 1;

    
    return mat;
}

Mat4 GetRotationMatrix(const Vec3 Rotation) {
    return
        GetRotationXMatrixEuler(Rotation.x) *
        GetRotationYMatrixEuler(Rotation.y) *
        GetRotationZMatrixEuler(Rotation.z);
}

Mat4 GetRotationMatrix(const Quat Rotation) {
    float x2 = Rotation.x * Rotation.x;
    float y2 = Rotation.y * Rotation.y;
    float z2 = Rotation.z * Rotation.z;
    float xy = Rotation.x * Rotation.y;
    float xz = Rotation.x * Rotation.z;
    float yz = Rotation.y * Rotation.z;
    float wx = Rotation.w * Rotation.x;
    float wy = Rotation.w * Rotation.y;
    float wz = Rotation.w * Rotation.z;

    Mat4 rot = Mat4(
        Vec4(1.0f - 2.0f * (y2 + z2), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f),
        Vec4(2.0f * (xy + wz), 1.0f - 2.0f * (x2 + z2), 2.0f * (yz - wx), 0.0f),
        Vec4(2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (x2 + y2), 0.0f),
        Vec4(0.0f, 0.0f, 0.0f, 1.0f));
    return rot;
}

Mat4 GetTransform(const Vec3& pos, const Vec3& scale, const Vec3& rotation) {
    Mat4 Result = 
        GetScaleMatrix(scale) * 
        GetRotationMatrix(rotation) *
        GetTranslationMatrix(pos)
        ;
    return Result;
}

Mat4 GetTransform(const Vec3& pos, const Vec3& scale, const Quat& rotation) {
    Mat4 Result =
        GetScaleMatrix(scale) *
        GetRotationMatrix(rotation) *
        GetTranslationMatrix(pos)
        ;
    return Result;
}

Mat4 Translate(Mat4& origin, const Vec3& translation) {
    Mat4 result = origin;

    result.Values[3][0] += translation.x;
    result.Values[3][1] += translation.y;
    result.Values[3][2] += translation.z;

    return result;
}
Mat4 Scale(Mat4& origin, const Vec3& scale) {
    Mat4 result = origin;

    result.Values[0][0] *= scale.x;
    result.Values[1][1] *= scale.y;
    result.Values[2][2] *= scale.z;

    return result;
}
Mat4 RemoveRotationFromTransform(const Mat4& transform, const Mat4& camera_view) {
    Mat4 result = transform;

    result.Values[0][0] = camera_view.Values[0][0];
    result.Values[0][1] = camera_view.Values[1][0];
    result.Values[0][2] = camera_view.Values[2][0];

    result.Values[1][0] = camera_view.Values[0][1];
    result.Values[1][1] = camera_view.Values[1][1];
    result.Values[1][2] = camera_view.Values[2][1];

    result.Values[2][0] = camera_view.Values[0][2];
    result.Values[2][1] = camera_view.Values[1][2];
    result.Values[2][2] = camera_view.Values[2][2];

    return result;
}