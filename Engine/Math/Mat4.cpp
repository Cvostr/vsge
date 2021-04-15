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