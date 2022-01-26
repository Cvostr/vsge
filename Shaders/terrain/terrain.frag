#version 450

layout (location = 0) out vec4 tColor;
layout (location = 1) out vec4 tNormal;
layout (location = 2) out vec3 tPos;
layout (location = 3) out vec4 tMaterial;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 InNormal;
layout(location = 2) in vec2 UVCoord;
layout(location = 3) in mat3 TBN;

#define MAX_TEXTURES 16
#define WIDTH 500
#define HEIGHT 500

struct MaterialFactors{
    float roughness_factor;
    float metallic_factor;
    float height_factor;
};

layout (std140, set = 1, binding = 0) uniform TerrainData{
    float texture_uv_x;
    float texture_uv_y;
    uint textures_count;

    MaterialFactors factors[MAX_TEXTURES];
};

layout (std140, binding = 0) uniform CamMatrices{
    mat4 cam_view_projection;
    mat4 cam_view;
    mat4 cam_projection;
    vec3 cam_position;
};

layout(set = 1, binding = 1) uniform sampler2DArray masks;
layout(set = 1, binding = 2) uniform sampler2D albedo[MAX_TEXTURES];
layout(set = 1, binding = 3) uniform sampler2D normal[MAX_TEXTURES];
layout(set = 1, binding = 4) uniform sampler2D roughness[MAX_TEXTURES];
layout(set = 1, binding = 5) uniform sampler2D metallic[MAX_TEXTURES];
layout(set = 1, binding = 6) uniform sampler2D ao[MAX_TEXTURES];
layout(set = 1, binding = 7) uniform sampler2D height[MAX_TEXTURES];

vec3 result_albedo;
vec3 result_normal;
float result_roughness;
float result_metallic;
float result_ao;

float GetFactor(vec2 uv, uint texture_id){
    return texture(masks, vec3(uv, texture_id / 4))[texture_id % 4];
}

vec3 GetAlbedo(vec2 uv, uint texture_id){
    return texture(albedo[texture_id], uv).rgb;
}

vec3 GetNormal(vec2 uv, uint texture_id){
    return texture(normal[texture_id], uv).rgb;
}

float GetRoughness(vec2 uv, uint texture_id){
    return texture(roughness[texture_id], uv).r;
}

float GetMetallic(vec2 uv, uint texture_id){
    return texture(metallic[texture_id], uv).r;
}

float GetAo(vec2 uv, uint texture_id){
    return texture(ao[texture_id], uv).r;
}

float GetHeight(vec2 uv, uint texture_id){
    return texture(height[texture_id], uv).r;
}

vec2 CalcParallaxOcclusion(vec2 uv, vec3 view_dir, uint texture_id){
    // number of depth layers
    const float minLayers = 8;
    const float maxLayers = 64;
    float height_factor = factors[texture_id].height_factor;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), view_dir)));  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = view_dir.xy / view_dir.z * height_factor; 
    vec2 deltaTexCoords = P / numLayers;
    // get initial values
    vec2  currentTexCoords     = uv;
    float currentDepthMapValue = GetHeight(currentTexCoords, texture_id);
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = GetHeight(currentTexCoords, texture_id);  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = GetHeight(prevTexCoords, texture_id) - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

void CalculateTextures(vec2 uv){
    vec2 terrain_uv = vec2(0);
    terrain_uv.x = uv.x * texture_uv_x;
    terrain_uv.y = uv.y * texture_uv_y;

    result_albedo = vec3(0);
    result_normal = vec3(0);
    result_roughness = 0;
    result_metallic = 0;
    result_ao = 1;

    for(uint i = 0; i < textures_count; i ++){
        float factor = GetFactor(uv, i);
        if(factor > 0.001){
            //height
            mat3 transposed_tbn = transpose(TBN);
            vec3 view_dir = normalize(transposed_tbn * cam_position - transposed_tbn * FragPos);
            terrain_uv = CalcParallaxOcclusion(terrain_uv, view_dir, i);

            //albedo
            vec3 tex_sample = GetAlbedo(terrain_uv, i);
            result_albedo += tex_sample * factor;

            //normal
            vec3 normal = GetNormal(terrain_uv, i);
            if(normal == vec3(0))
                normal = InNormal;
            else{
                normal = normalize(normal * 2 - 1);
                normal = normalize(TBN * normal);
            }
            result_normal += normal * factor;
            //roughness
            float roughness_sample = GetRoughness(terrain_uv, i) * factors[i].roughness_factor;
            result_roughness += roughness_sample * factor;
            //metallic
            float metallic_sample = GetMetallic(terrain_uv, i) * factors[i].metallic_factor;
            result_metallic += metallic_sample * factor;
            //ao
            float ao_sample = GetAo(terrain_uv, i);
            result_ao += ao_sample * factor;
        }
    }
}

void main() {
    CalculateTextures(UVCoord);
    tColor = vec4(result_albedo, result_ao);
    tNormal = vec4(result_normal, result_roughness);
    tPos = FragPos;
    tMaterial = vec4(0, 0, 0, result_metallic);
}