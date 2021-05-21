layout (location = 0) out vec4 tColor;
layout (location = 1) out vec3 tNormal;
layout (location = 2) out vec3 tPos;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 InNormal;
layout(location = 2) in vec2 UVCoord;
layout(location = 3) in mat3 TBN;
/*
layout (std140, binding = 3) uniform Material {
    uniform mat4 bone_transform[400];
};*/

layout(set = 1, binding = 1) uniform sampler2D diffuse;
//layout(set = 1, binding = 2) uniform sampler2D normal;

void main() {
  //  tColor = texture(diffuse, UVCoord);
    //vec3 normal = texture(normal, UVCoord).rgb;
    tColor = texture(diffuse, UVCoord);
}   
