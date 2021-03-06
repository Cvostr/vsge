#version 450

layout(location = 0) out vec4 tColor;

layout(location = 0) in vec2 UVCoord;

layout(set=0, binding=0) uniform sampler2D positions;
layout(set=0, binding=1) uniform sampler2D normals;
layout(set=0, binding=2) uniform sampler2D noise;

layout (std140, set = 0, binding = 3) uniform Buffer{
    vec3 samples[64];
};

layout (std140, binding = 4) uniform CamMatrices{
    mat4 cam_view_projection;
    mat4 cam_view;
    mat4 cam_projection;
    vec3 cam_position;
};

void main() {
    vec2 noise_uv_scale = textureSize(positions, 0) / 4.0;
    vec4 pos = texture(positions, UVCoord);
    vec4 normal_map = texture(normals, UVCoord);
    if(pos.a == 1.0){
        discard;
    }
    vec3 fragPos = pos.xyz;
    fragPos = (cam_view * vec4(fragPos, 1)).xyz;
    vec3 normal = normalize(normal_map.rgb);
    vec3 randomVec = texture(noise, UVCoord * noise_uv_scale).xyz;  

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    int kernelSize = 64;
    float radius = 0.9;
    float bias = 0.03;

    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i){
        vec3 samplePos = TBN * samples[i]; // from tangent to view-space
        samplePos = fragPos + samplePos * radius; 
        vec4 offset = vec4(samplePos, 1.0);
        offset      = cam_projection * offset;    
        offset.xyz /= offset.w;               
        offset.xyz  = offset.xyz * 0.5 + 0.5;
        float sampleDepth = (cam_view * vec4(texture(positions, offset.xy).rgb, 1)).z;  
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;       
    }
    occlusion = 1.0 - (occlusion / kernelSize); 
    tColor = vec4(occlusion, 0, 0, 1);    
}