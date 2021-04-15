#pragma once

#include <Core/VarTypes/Base.hpp>
#include "Vec4.hpp"

float determinant(float a, float b, float c, float d,
                  float e, float f, float g, float h, float i);

template<typename T>
class tMat4 {
public:
    union
    {
        struct
        {
            //Row 1
            T M11, M12, M13, M14;
            //Row 2
            T M21, M22, M23, M24;
            //Row3
            T M31, M32, M33, M34;
            //Row 4
            T M41, M42, M43, M44;
        };

        T Values[4][4];
        T Raw[16];
    };
    tMat4(){
        for (uint8 i = 0; i < 16; i++)
            Raw[i] = 0;
    }

    tMat4(T a) {
            for (uint8 i = 0; i < 16; i++)
                Raw[i] = 0;
            for (uint8 i = 0; i < 4; i++)
                Values[i][i] = a;
    }

    tMat4 operator*(T r) const {
        tMat4 result = *this;
        for (uint8 i = 0; i < 4; i++) {
            for (uint8 b = 0; b < 4; b++) {
                result.Values[i][b] *= r;
            }
        }

        return result;
    }

    tMat4 operator+(tMat4 r) const {
        tMat4 result = *this;
        for (uint8 i = 0; i < 4; i++) {
            for (uint8 b = 0; b < 4; b++) {
                result.Values[i][b] += r.Values[i][b];
            }
        }

        return result;
    }

    tMat4 operator*(tMat4 r) const {
        tMat4 Ret;
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

    tVec4<T> operator*(tVec4<T> r) const {
        tVec4<T> Ret;
        Ret.x = r.x * Values[0][0] + r.y * Values[1][0] + r.z * Values[2][0] + r.w * Values[3][0];
        Ret.y = r.x * Values[0][1] + r.y * Values[1][1] + r.z * Values[2][1] + r.w * Values[3][1];
        Ret.z = r.x * Values[0][2] + r.y * Values[1][2] + r.z * Values[2][2] + r.w * Values[3][2];
        Ret.w = r.x * Values[0][3] + r.y * Values[1][3] + r.z * Values[2][3] + r.w * Values[3][3];
        return Ret;
    }

    tMat4 transpose() {
        tMat4 result;

        for (uint32 i = 0; i < 4; i++) {
            for (uint32 j = 0; j < 4; j++) {
                result.Values[i][j] = Values[j][i];
            }
        }
        return result;
    }

    tMat4 invert() {
        float det_inv = 1.f / determinant();

        float A11 = +determinant(Values[1][1], Values[1][2], Values[1][3], Values[2][1], Values[2][2], Values[2][3], Values[3][1], Values[3][2], Values[3][3]);
        float A12 = -determinant(Values[1][0], Values[1][2], Values[1][3], Values[2][0], Values[2][2], Values[2][3], Values[3][0], Values[3][2], Values[3][3]);
        float A13 = +determinant(Values[1][0], Values[1][1], Values[1][3], Values[2][0], Values[2][1], Values[2][3], Values[3][0], Values[3][1], Values[3][3]);
        float A14 = -determinant(Values[1][0], Values[1][1], Values[1][2], Values[2][0], Values[2][1], Values[2][2], Values[3][0], Values[3][1], Values[3][2]);
                                                                                                                     
        float A21 = -determinant(Values[0][1], Values[0][2], Values[0][3], Values[2][1], Values[2][2], Values[2][3], Values[3][1], Values[3][2], Values[3][3]);
        float A22 = +determinant(Values[0][0], Values[0][2], Values[0][3], Values[2][0], Values[2][2], Values[2][3], Values[3][0], Values[3][2], Values[3][3]);
        float A23 = -determinant(Values[0][0], Values[0][1], Values[0][3], Values[2][0], Values[2][1], Values[2][3], Values[3][0], Values[3][1], Values[3][3]);
        float A24 = +determinant(Values[0][0], Values[0][1], Values[0][2], Values[2][0], Values[2][1], Values[2][2], Values[3][0], Values[3][1], Values[3][2]);
                                                                                                                     
        float A31 = +determinant(Values[0][1], Values[0][2], Values[0][3], Values[1][1], Values[1][2], Values[1][3], Values[3][1], Values[3][2], Values[3][3]);
        float A32 = -determinant(Values[0][0], Values[0][2], Values[0][3], Values[1][0], Values[1][2], Values[1][3], Values[3][0], Values[3][2], Values[3][3]);
        float A33 = +determinant(Values[0][0], Values[0][1], Values[0][3], Values[1][0], Values[1][1], Values[1][3], Values[3][0], Values[3][1], Values[3][3]);
        float A34 = -determinant(Values[0][0], Values[0][1], Values[0][2], Values[1][0], Values[1][1], Values[1][2], Values[3][0], Values[3][1], Values[3][2]);
                                                                                                                     
        float A41 = -determinant(Values[0][1], Values[0][2], Values[0][3], Values[1][1], Values[1][2], Values[1][3], Values[2][1], Values[2][2], Values[2][3]);
        float A42 = +determinant(Values[0][0], Values[0][2], Values[0][3], Values[1][0], Values[1][2], Values[1][3], Values[2][0], Values[2][2], Values[2][3]);
        float A43 = -determinant(Values[0][0], Values[0][1], Values[0][3], Values[1][0], Values[1][1], Values[1][3], Values[2][0], Values[2][1], Values[2][3]);
        float A44 = +determinant(Values[0][0], Values[0][1], Values[0][2], Values[1][0], Values[1][1], Values[1][2], Values[2][0], Values[2][1], Values[2][2]);
        
        Mat4 result = Mat4(A11, A21, A31, A41,
                            A12, A22, A32, A42,
                            A13, A23, A33, A43,
                            A14, A24, A34, A44);
        result = result * det_inv;
        return result;
    }

    float determinant() {
        float d1 = Values[0][0] * determinant(Values[1][1], Values[1][2], Values[1][3],
            Values[2][1], Values[2][2], Values[2][3], Values[3][1], Values[3][2], Values[3][3]);
        float d2 = Values[0][1] * determinant(Values[1][0], Values[1][2], Values[1][3],
            Values[2][0], Values[2][2], Values[2][3], Values[3][0], Values[3][2], Values[3][3]);
        float d3 = Values[0][2] * determinant(Values[1][0], Values[1][1], Values[1][3],
            Values[2][0], Values[2][1], Values[2][3], Values[3][0], Values[3][1], Values[3][3]);
        float d4 = Values[0][3] * determinant(Values[1][0], Values[1][1], Values[1][2],
            Values[2][0], Values[2][1], Values[2][2], Values[3][0], Values[3][1], Values[3][2]);

        return d1 - d2 + d3 - d4;
    }

};

typedef tMat4<float> Mat4;
