layout (location = 0) out vec4 tColor;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 InNormal;
layout(location = 2) in vec3 n_depth;
layout(location = 3) in vec2 UVCoord;
layout(location = 4) in mat3 TBN;

layout(set = 1, binding = 1) uniform sampler2D albedo;
layout(set = 1, binding = 2) uniform sampler2D normal_map;
layout(set = 1, binding = 3) uniform sampler2D roughness_map;
layout(set = 1, binding = 4) uniform sampler2D metallic_map;
layout(set = 1, binding = 5) uniform sampler2D occlusion_map;
layout(set = 1, binding = 6) uniform sampler2D emission_map;

layout(set = 3, binding = 8) uniform sampler2D depth;

layout (std140, set = 0, binding = 0) uniform CamMatrices{
    mat4 cam_view_projection;
    mat4 cam_view;
    mat4 cam_projection;
    vec3 cam_position;
};

layout (std140, set = 1, binding = 0) uniform MaterialData{
    bool hasAlbedo;
    bool hasNormal;
    bool hasRoughness;
    bool hasMetallic;
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

void main() {
    float old_depth = texture(depth, n_depth.xy * 0.5 + 0.5).r;
    if(old_depth <= n_depth.z)
        discard;

    tColor = vec4(color.xyz, 1);
    vec3 normal = InNormal;
    vec2 uv_coords = UVCoord;

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
}   
