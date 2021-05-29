#include "Mat4.hpp"

static const float RAD2DEG = (180.f / 3.14159265f);

template<typename T>
tMat4<T> tMat4<T>::operator+(tMat4<T> r) const {
    tMat4<T> result = *this;
    for (uint8 i = 0; i < 4; i++) {
        for (uint8 b = 0; b < 4; b++) {
            result.Values[i][b] += r.Values[i][b];
        }
    }

    return result;
}

template<typename T>
tMat4<T> tMat4<T>::operator*(tMat4<T> r) const {
    tMat4<T> Ret;
    for (uint8 i = 0; i < 4; i++) {
        for (uint8 b = 0; b < 4; b++) {
            Ret.Values[i][b] =
                Values[i][0] * r.Values[0][b] +
                Values[i][1] * r.Values[1][b] +
                Values[i][2] * r.Values[2][b] +
                Values[i][3] * r.Values[3][b];
        }
    }

    return Ret;
}

template<typename T>
tMat4<T> tMat4<T>::operator*(T r) const {
    tMat4<T> result = *this;
    for (uint8 i = 0; i < 4; i++) {
        for (uint8 b = 0; b < 4; b++) {
            result.Values[i][b] *= r;
        }
    }

    return result;
}

template<typename T>
tVec4<T> tMat4<T>::operator*(tVec4<T> r) const {
    tVec4<T> Ret;
    Ret.x = r.x * Values[0][0] + r.y * Values[1][0] + r.z * Values[2][0] + r.w * Values[3][0];
    Ret.y = r.x * Values[0][1] + r.y * Values[1][1] + r.z * Values[2][1] + r.w * Values[3][1];
    Ret.z = r.x * Values[0][2] + r.y * Values[1][2] + r.z * Values[2][2] + r.w * Values[3][2];
    Ret.w = r.x * Values[0][3] + r.y * Values[1][3] + r.z * Values[2][3] + r.w * Values[3][3];
    return Ret;
}

template<typename T>
bool tMat4<T>::operator==(tMat4 r)const {
    for (uint8 i = 0; i < 16; i++)
        if(Raw[i] != r.Raw[i])
            return false;
    return true;
}

template<typename T>
tMat4<T> tMat4<T>::transpose() const {
    tMat4<T> result;

    for (uint32 i = 0; i < 4; i++) {
        for (uint32 j = 0; j < 4; j++) {
            result.Values[i][j] = Values[j][i];
        }
    }
    return result;
}

template<typename T>
tMat4<T> tMat4<T>::invert() {
    float det_inv = 1.f / determinant();

    T A11 = +_determinant(Values[1][1], Values[1][2], Values[1][3], Values[2][1], Values[2][2], Values[2][3], Values[3][1], Values[3][2], Values[3][3]);
    T A12 = -_determinant(Values[1][0], Values[1][2], Values[1][3], Values[2][0], Values[2][2], Values[2][3], Values[3][0], Values[3][2], Values[3][3]);
    T A13 = +_determinant(Values[1][0], Values[1][1], Values[1][3], Values[2][0], Values[2][1], Values[2][3], Values[3][0], Values[3][1], Values[3][3]);
    T A14 = -_determinant(Values[1][0], Values[1][1], Values[1][2], Values[2][0], Values[2][1], Values[2][2], Values[3][0], Values[3][1], Values[3][2]);
                 
    T A21 = -_determinant(Values[0][1], Values[0][2], Values[0][3], Values[2][1], Values[2][2], Values[2][3], Values[3][1], Values[3][2], Values[3][3]);
    T A22 = +_determinant(Values[0][0], Values[0][2], Values[0][3], Values[2][0], Values[2][2], Values[2][3], Values[3][0], Values[3][2], Values[3][3]);
    T A23 = -_determinant(Values[0][0], Values[0][1], Values[0][3], Values[2][0], Values[2][1], Values[2][3], Values[3][0], Values[3][1], Values[3][3]);
    T A24 = +_determinant(Values[0][0], Values[0][1], Values[0][2], Values[2][0], Values[2][1], Values[2][2], Values[3][0], Values[3][1], Values[3][2]);
                 
    T A31 = +_determinant(Values[0][1], Values[0][2], Values[0][3], Values[1][1], Values[1][2], Values[1][3], Values[3][1], Values[3][2], Values[3][3]);
    T A32 = -_determinant(Values[0][0], Values[0][2], Values[0][3], Values[1][0], Values[1][2], Values[1][3], Values[3][0], Values[3][2], Values[3][3]);
    T A33 = +_determinant(Values[0][0], Values[0][1], Values[0][3], Values[1][0], Values[1][1], Values[1][3], Values[3][0], Values[3][1], Values[3][3]);
    T A34 = -_determinant(Values[0][0], Values[0][1], Values[0][2], Values[1][0], Values[1][1], Values[1][2], Values[3][0], Values[3][1], Values[3][2]);
                 
    T A41 = -_determinant(Values[0][1], Values[0][2], Values[0][3], Values[1][1], Values[1][2], Values[1][3], Values[2][1], Values[2][2], Values[2][3]);
    T A42 = +_determinant(Values[0][0], Values[0][2], Values[0][3], Values[1][0], Values[1][2], Values[1][3], Values[2][0], Values[2][2], Values[2][3]);
    T A43 = -_determinant(Values[0][0], Values[0][1], Values[0][3], Values[1][0], Values[1][1], Values[1][3], Values[2][0], Values[2][1], Values[2][3]);
    T A44 = +_determinant(Values[0][0], Values[0][1], Values[0][2], Values[1][0], Values[1][1], Values[1][2], Values[2][0], Values[2][1], Values[2][2]);

    tMat4<T> result = tMat4<T>(tVec4<T>(A11, A21, A31, A41),
        tVec4<T>(A12, A22, A32, A42),
        tVec4<T>(A13, A23, A33, A43),
        tVec4<T>(A14, A24, A34, A44));
    result = result * det_inv;
    return result;
}

template<typename T>
T tMat4<T>::determinant() const {
    T d1 = Values[0][0] * _determinant(Values[1][1], Values[1][2], Values[1][3],
        Values[2][1], Values[2][2], Values[2][3], Values[3][1], Values[3][2], Values[3][3]);
    T d2 = Values[0][1] * _determinant(Values[1][0], Values[1][2], Values[1][3],
        Values[2][0], Values[2][2], Values[2][3], Values[3][0], Values[3][2], Values[3][3]);
    T d3 = Values[0][2] * _determinant(Values[1][0], Values[1][1], Values[1][3],
        Values[2][0], Values[2][1], Values[2][3], Values[3][0], Values[3][1], Values[3][3]);
    T d4 = Values[0][3] * _determinant(Values[1][0], Values[1][1], Values[1][2],
        Values[2][0], Values[2][1], Values[2][2], Values[3][0], Values[3][1], Values[3][2]);

    return d1 - d2 + d3 - d4;
}

template<typename T>
tVec3<T> tMat4<T>::GetScale() const { 
    float x = tVec3<T>(M11, M12, M13).Length();
    float y = tVec3<T>(M21, M22, M23).Length();
    float z = tVec3<T>(M31, M32, M33).Length();
    return tVec3<T>(x, y, z); 
}

template<typename T>
tVec3<T> tMat4<T>::GetRotation() const {
    tVec3<T> rot;
    tVec3<T> scale = GetScale();
    tMat4<T> m = *this;
    m.M11 /= scale.x;
    m.M12 /= scale.x;
    m.M13 /= scale.x;

    m.M21 /= scale.y;
    m.M22 /= scale.y;
    m.M23 /= scale.y;

    m.M31 /= scale.z;
    m.M32 /= scale.z;
    m.M33 /= scale.z;

    rot.x = (atan2f(Values[1][2], Values[2][2]));
    rot.y = (atan2f(-Values[0][2], sqrtf(Values[1][2] * Values[1][2] + Values[2][2] * Values[2][2])));
    rot.z = (atan2f(Values[0][1], Values[0][0]));

    return rot * RAD2DEG;
}

Quat GetRotation(const Mat4& mat) {

    Vec3 scale = mat.GetScale();

    // Avoid division by zero (we'll divide to remove scaling)
    if (scale.x == 0.0f || scale.y == 0.0f || scale.z == 0.0f) { return Quat(0, 0, 0, 1); }

    // Extract rotation and remove scaling
    Mat4 normalized;
    normalized.M11 = mat.M11 / scale.x; normalized.M12 = mat.M12 / scale.x; normalized.M13 = mat.M13 / scale.x; normalized.M14 = 0.0f;
    normalized.M21 = mat.M21 / scale.y; normalized.M22 = mat.M22 / scale.y; normalized.M23 = mat.M23 / scale.y; normalized.M24 = 0.0f;
    normalized.M31 = mat.M31 / scale.z; normalized.M32 = mat.M32 / scale.z; normalized.M33 = mat.M33 / scale.z; normalized.M34 = 0.0f;
    normalized.M41 = 0; normalized.M42 = 0; normalized.M43 = 0; normalized.M44 = 1.0f;

    Quat quaternion;
    float sqrt;
    float half;
    float _scale = normalized.M11 + normalized.M22 + normalized.M33;

    if (_scale > 0.0f)
    {
        sqrt = sqrtf(_scale + 1.0f);
        quaternion.w = sqrt * 0.5f;
        sqrt = 0.5f / sqrt;

        quaternion.x = (normalized.M23 - normalized.M32) * sqrt;
        quaternion.y = (normalized.M31 - normalized.M13) * sqrt;
        quaternion.z = (normalized.M12 - normalized.M21) * sqrt;

        return quaternion;
    }
    if ((normalized.M11 >= normalized.M22) && (normalized.M11 >= normalized.M33))
    {
        sqrt = sqrtf(1.0f + normalized.M11 - normalized.M22 - normalized.M33);
        half = 0.5f / sqrt;

        quaternion.x = 0.5f * sqrt;
        quaternion.y = (normalized.M12 + normalized.M21) * half;
        quaternion.z = (normalized.M13 + normalized.M31) * half;
        quaternion.w = (normalized.M23 - normalized.M32) * half;

        return quaternion;
    }
    if (normalized.M22 > normalized.M33)
    {
        sqrt = sqrtf(1.0f + normalized.M22 - normalized.M11 - normalized.M33);
        half = 0.5f / sqrt;

        quaternion.x = (normalized.M21 + normalized.M12) * half;
        quaternion.y = 0.5f * sqrt;
        quaternion.z = (normalized.M32 + normalized.M23) * half;
        quaternion.w = (normalized.M31 - normalized.M13) * half;

        return quaternion;
    }
    sqrt = sqrtf(1.0f + normalized.M33 - normalized.M11 - normalized.M22);
    half = 0.5f / sqrt;

    quaternion.x = (normalized.M31 + normalized.M13) * half;
    quaternion.y = (normalized.M32 + normalized.M23) * half;
    quaternion.z = 0.5f * sqrt;
    quaternion.w = (normalized.M12 - normalized.M21) * half;

    return quaternion;
}

template class tMat4<float>;
template class tMat4<int>;