layout (location = 0) out vec4 tColor;
layout (location = 1) out vec3 tNormal;
layout (location = 2) out vec3 tPos;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 InNormal;
layout(location = 2) in vec2 UVCoord;
layout(location = 3) in mat3 TBN;

layout(set = 1, binding = 1) uniform sampler2D albedo;
layout(set = 1, binding = 2) uniform sampler2D normal;
layout(set = 1, binding = 3) uniform sampler2D roughness;
layout(set = 1, binding = 4) uniform sampler2D metallic;
layout(set = 1, binding = 5) uniform sampler2D height;
layout(set = 1, binding = 6) uniform sampler2D occlusion;

layout (std140, set = 1, binding = 0) uniform MaterialData{
    bool hasAlbedo;
    bool hasNormal;
    bool hasRoughness;
    bool hasMetallic;
    bool hasHeight;
    bool hasOcclusion;
};

void main() {
    tColor = vec4(1, 1, 1, 1);
    tNormal = InNormal;

    if(hasAlbedo)
        tColor = texture(albedo, UVCoord);

    if(hasNormal){
        tNormal = texture(normal, UVCoord).rgb;
        tNormal = normalize(tNormal * 2 - 1);
		tNormal = normalize(TBN * tNormal);
    }

    tPos = FragPos;
}   
