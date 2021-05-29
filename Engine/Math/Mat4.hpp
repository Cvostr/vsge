#pragma once

#include <Core/VarTypes/Base.hpp>
#include "Vec4.hpp"

template<typename T>
T _determinant(T a, T b, T c, T d, T e, T f,
    T g, T h, T i) {
    //a b c
    //d e f
    //g h i
    return (a * e * i) +
        (d * c * h) +
        (g * b * f) -
        (g * e * c) -
        (d * b * i) -
        (h * f * a);
}

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
        tVec4<T> Rows[4];
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

    tMat4(tVec4<T> v1, tVec4<T> v2, tVec4<T> v3, tVec4<T> v4) {
        Values[0][0] = v1.x; Values[0][1] = v1.y; Values[0][2] = v1.z; Values[0][3] = v1.w;
        Values[1][0] = v2.x; Values[1][1] = v2.y; Values[1][2] = v2.z; Values[1][3] = v2.w;
        Values[2][0] = v3.x; Values[2][1] = v3.y; Values[2][2] = v3.z; Values[2][3] = v3.w;
        Values[3][0] = v4.x; Values[3][1] = v4.y; Values[3][2] = v4.z; Values[3][3] = v4.w;
    }

    tVec4<T>& operator[](uint32 i) {
        return Rows[i];
    }

    bool operator==(tMat4 r)const;

    tMat4 operator*(T r) const;

    tMat4 operator+(tMat4 r) const;

    tMat4 operator*(tMat4 r) const ;

    tVec4<T> operator*(tVec4<T> r) const;

    tVec3<T> GetPosition() const { return tVec3<T>(Values[3][0], Values[3][1], Values[3][2]); }
    tVec3<T> GetScale() const;
    tVec3<T> GetRotation() const;

    tMat4 transpose() const;

    tMat4 invert();

    T determinant() const ;

};

typedef tMat4<float> Mat4;
