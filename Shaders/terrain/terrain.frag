layout (location = 0) out vec4 tColor;
layout (location = 1) out vec3 tNormal;
layout (location = 2) out vec3 tPos;
layout (location = 3) out vec4 tMaterial;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 InNormal;
layout(location = 2) in vec2 UVCoord;
layout(location = 3) in mat3 TBN;

//layout (std140, set = 1, binding = 0) uniform MaterialData{

//};

#define MAX_TEXTURES 16

layout(binding = 1) uniform sampler2D masks;
layout(binding = 2) uniform sampler2D albedo[MAX_TEXTURES];
layout(binding = 3) uniform sampler2D normal[MAX_TEXTURES];
layout(binding = 4) uniform sampler2D roughness[MAX_TEXTURES];
layout(binding = 5) uniform sampler2D metallic[MAX_TEXTURES];

float CalcLuminance(vec3 color)
{
    return dot(color, vec3(0.299, 0.587, 0.114));
}

void main() {
    vec3 normal = InNormal;

    tColor = vec4(1, 1, 1, 1);
    tNormal = normal;
    tPos = FragPos;
    tMaterial = vec4(1, 0, 0, 1);
}   
