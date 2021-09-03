layout (location = 0) out vec4 tColor;
layout (location = 1) out vec3 tNormal;
layout (location = 2) out vec3 tPos;
layout (location = 3) out vec4 tMaterial;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 InNormal;
layout(location = 2) in vec2 UVCoord;
layout(location = 3) in mat3 TBN;

layout(set = 1, binding = 1) uniform sampler2D albedo;
layout(set = 1, binding = 2) uniform sampler2D normal_map;
layout(set = 1, binding = 3) uniform sampler2D roughness_map;
layout(set = 1, binding = 4) uniform sampler2D metallic_map;
layout(set = 1, binding = 5) uniform sampler2D height_map;
layout(set = 1, binding = 6) uniform sampler2D occlusion_map;
layout(set = 1, binding = 7) uniform sampler2D emission_map;

layout (std140, set = 0, binding = 0) uniform CamMatrices{
    mat4 cam_view_projection;
    mat4 cam_view;
    vec3 cam_position;
};

layout (std140, set = 1, binding = 0) uniform MaterialData{
    bool hasAlbedo;
    bool hasNormal;
    bool hasRoughness;
    bool hasMetallic;
    bool hasHeight;
    bool hasOcclusion;
    bool hasEmission;

    vec4 color;
    float roughness_factor;
    float metallic_factor;
    float height_factor;
};

float CalcLuminance(vec3 color)
{
    return dot(color, vec3(0.299, 0.587, 0.114));
}

vec2 CalcParallaxOcclusion(vec2 uv, vec3 view_dir){
    float height = texture(height_map, uv).r;
    vec2 p = view_dir.xy / view_dir.z * (height * height_factor);
    return uv - p;
}

void main() {
    tColor = vec4(color.xyz, 1);
    vec3 normal = InNormal;
    
    vec2 uv_coords = UVCoord;

    if(hasHeight){
        vec3 view_dir = normalize(TBN * cam_position - TBN * FragPos);
        uv_coords = CalcParallaxOcclusion(UVCoord, view_dir);
    }

    if(hasAlbedo)
        tColor *= texture(albedo, uv_coords);

    if(hasNormal){
        normal = texture(normal_map, uv_coords).rgb;
        normal = normalize(normal * 2 - 1);
		normal = normalize(TBN * normal);
    }

    float roughness = roughness_factor;
    float metallic = metallic_factor;
    float emission = 0.0;
    float ao = 1.0;

    if(hasRoughness){
        roughness *= texture(roughness_map, uv_coords).r;
    }

    if(hasMetallic){
        metallic *= texture(metallic_map, uv_coords).r;
    }

    if(hasEmission){
        CalcLuminance(texture(emission_map, uv_coords).rgb);
    }

    if(hasOcclusion)
        ao = texture(occlusion_map, uv_coords).r;

    tNormal = normal;
    tPos = FragPos;
    tMaterial = vec4(roughness, metallic, emission, ao);
}   
