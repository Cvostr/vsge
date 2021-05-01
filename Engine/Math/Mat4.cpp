#include "Mat4.hpp"

float determinant(float a, float b, float c, float d, float e, float f,
                  float g, float h, float i) {
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
tMat4<T> tMat4<T>::invert() {
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

    tMat4 result = tMat4(A11, A21, A31, A41,
        A12, A22, A32, A42,
        A13, A23, A33, A43,
        A14, A24, A34, A44);
    result = result * det_inv;
    return result;
}

template<typename T>
float tMat4<T>::determinant() {
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