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
    mat4 cam_projection;
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
    float emission_factor;
};

float CalcLuminance(vec3 color)
{
    return dot(color, vec3(0.299, 0.587, 0.114));
}

vec2 CalcParallaxOcclusion(vec2 uv, vec3 view_dir){
    // number of depth layers
    const float minLayers = 8;
    const float maxLayers = 64;
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
    float currentDepthMapValue = texture(height_map, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(height_map, currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(height_map, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

void main() {
    tColor = vec4(color.xyz, 1);
    vec3 normal = InNormal;
    
    vec2 uv_coords = UVCoord;

    if(hasHeight){
        mat3 transposed_tbn = transpose(TBN);
        vec3 view_dir = normalize(transposed_tbn * cam_position - transposed_tbn * FragPos);
        uv_coords = CalcParallaxOcclusion(UVCoord, view_dir);

        if(uv_coords.x > 1.0 || uv_coords.y > 1.0 || uv_coords.x < 0.0 || uv_coords.y < 0.0)
            discard;
    }

    if(hasAlbedo)
        tColor *= vec4(texture(albedo, uv_coords).rgb, 1);

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
        emission = CalcLuminance(texture(emission_map, uv_coords).rgb) * emission_factor;
    }

    if(hasOcclusion)
        ao = texture(occlusion_map, uv_coords).r;

    tNormal = normal;
    tPos = FragPos;
    tMaterial = vec4(roughness, metallic, emission / 10.0, ao);
}   
