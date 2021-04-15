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