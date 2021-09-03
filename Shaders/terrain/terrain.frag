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

#define WIDTH 500
#define HEIGHT 500

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

float CalcLuminance(vec3 color)
{
    return dot(color, vec3(0.299, 0.587, 0.114));
}

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

void CalculateTextures(vec2 uv){
    vec2 terrain_uv = vec2(0);
    terrain_uv.x = uv.x * WIDTH / 64;
    terrain_uv.y = uv.y * HEIGHT / 64;

    result_albedo = vec3(0);
    result_normal = vec3(0);
    result_roughness = 1;
    result_metallic = 0;
    result_ao = 1;

    for(int i = 0; i < MAX_TEXTURES; i ++){
        float factor = GetFactor(uv, i);
        //albedo
        vec3 tex_sample = GetAlbedo(terrain_uv, i);
        result_albedo = mix(result_albedo, tex_sample, factor);
        //normal
        vec3 normal = GetNormal(terrain_uv, i);
        if(normal == vec3(0))
            normal = InNormal;
        else{
            normal = normalize(normal * 2 - 1);
		    normal = normalize(TBN * normal);
        }
        result_normal = mix(result_normal, normal, factor);
        //roughness
        float roughness_sample = GetRoughness(terrain_uv, i);
        result_roughness = mix(result_roughness, roughness_sample, factor);
        //metallic
        float metallic_sample = GetMetallic(terrain_uv, i);
        result_metallic = mix(result_metallic, metallic_sample, factor);
        //ao
        float ao_sample = GetAo(terrain_uv, i);
        result_ao = mix(result_ao, ao_sample, factor);
    }
}

void main() {
    CalculateTextures(UVCoord);
    tColor = vec4(result_albedo, 1);
    tNormal = result_normal;
    tPos = FragPos;
    tMaterial = vec4(result_roughness, result_metallic, 0, result_ao);
}   
