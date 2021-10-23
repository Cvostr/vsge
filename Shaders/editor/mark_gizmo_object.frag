layout (location = 0) out vec4 tColor;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 InNormal;
layout(location = 2) in vec2 UVCoord;
layout(location = 3) in mat3 TBN;

void main() {
    tColor = vec4(0.5, 0.5, 0.5, 1);
}   
