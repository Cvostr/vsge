#pragma once

#include <Core/VarTypes/Base.hpp>
#include "Vec4.hpp"

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
                Ret.m[i][b] =
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

    tMat4 transpose(const tMat4& mat) {
        tMat4 result;

        for (uint32 i = 0; i < 4; i++) {
            for (uint32 j = 0; j < 4; j++) {
                result.Values[i][j] = mat.Values[j][i];
            }
        }
        return result;
    }

};

typedef tMat4<float> Mat4;
