#include "Mat4.hpp"

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

template class tMat4<float>;
template class tMat4<int>;