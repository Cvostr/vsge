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

vec3 GetRoughness(vec2 uv, uint texture_id){
    return texture(roughness[texture_id], uv).rgb;
}

vec3 GetMetallic(vec2 uv, uint texture_id){
    return texture(metallic[texture_id], uv).rgb;
}

vec3 GetAo(vec2 uv, uint texture_id){
    return texture(ao[texture_id], uv).rgb;
}

vec3 GetFragment(vec2 uv){
    vec3 result = vec3(0,0,0);

    vec2 nuv = uv;
    nuv.x *= WIDTH / 64;
    nuv.y *= HEIGHT / 64;

    for(int i = 0; i < MAX_TEXTURES; i ++){
        float factor = GetFactor(uv, i);
        vec3 diffuse = GetAlbedo(nuv, i);
        result = mix(result, diffuse, factor);
    }
        
    return result;
}

void main() {
    vec3 normal = InNormal;
    vec3 fragm = GetFragment(UVCoord);
    tColor = vec4(fragm, 1);
    tNormal = normal;
    tPos = FragPos;
    tMaterial = vec4(1, 0, 0, 1);
}   
