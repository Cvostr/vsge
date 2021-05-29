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