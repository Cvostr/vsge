layout (location = 0) out vec4 tColor;
layout (location = 1) out vec3 tNormal;
layout (location = 2) out vec3 tPos;
layout (location = 3) out vec4 tMaterial;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 InNormal;
layout(location = 2) in vec2 UVCoord;
layout(location = 3) in mat3 TBN;

layout(set = 1, binding = 1) uniform sampler2D albedo;

float CalcLuminance(vec3 color)
{
    return dot(color, vec3(0.299, 0.587, 0.114));
}

void main() {
    vec4 color = texture(albedo, UVCoord);
    if(color.a < 0.2)
        discard;

    tColor = vec4(color.rgb, 1);
    tNormal = InNormal;
    tPos = FragPos;
    tMaterial = vec4(1, 0, 0, 1);
}   
