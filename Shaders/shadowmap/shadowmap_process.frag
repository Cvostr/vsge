#version 450

layout(location = 0) out vec4 tColor;

layout(location = 0) in vec2 UVCoord;

struct CasterInfo{  //688
    mat4 projections[10];
    //640
    float ShadowBias; //4
    int ShadowmapSize; //4
    uint CascadesNum; //4
    uint PcfPassNum; // 4
    float ShadowStrength; // 4
    int caster_type; //4
    vec3 pos;
};

layout (std140, binding = 0) uniform ShadowData{
    CasterInfo casters[64];
    uint casters_count;
};

layout(binding = 1) uniform sampler2D gpos;

layout (std140, binding = 2) uniform CamMatrices{
    mat4 cam_view_projection;
    vec3 cam_position;
};

layout(binding = 3) uniform sampler2DArray shadowmaps[33];
layout(binding = 4) uniform samplerCube shadowmaps_point[32];

float sizes[] = { 40, 100, 160, 260, 320, 380, 460, 540 };

uint GetCascade(float dist){
    for(uint i = 0; i < 8; i ++){
        if(dist < (sizes[i] - 2))
            return i;
    }
    return 0;
}

float GetShadowmapSample(uint shadowmap, uint cascade, vec2 uv){
    return texture(shadowmaps[shadowmap], vec3(uv, cascade)).r; 
}

uint GetPointShadowTextureIndex(uint caster_index){
    uint pcaster_index = 0;
    for(uint i = 0; i < caster_index; i ++){
        if(casters[i].caster_type == 1)
            pcaster_index++;
    }
    return pcaster_index;
}

void main(){
    vec4 _pos = texture(gpos, UVCoord);
    vec3 FragPos = _pos.xyz;
    if(_pos.a == 1){
        discard;
    }
    
    float dist = length(FragPos - cam_position);
    float result = 0;
    
    for(uint caster_i = 0; caster_i < casters_count; caster_i ++){
        if(casters[caster_i].caster_type == 0){
            uint cascade = GetCascade(dist);
            if((cascade + 1) > casters[caster_i].CascadesNum)
                continue;
            
            vec4 objPosLightSpace = casters[caster_i].projections[cascade] * vec4(FragPos, 1);
            vec3 shadowProjection = (objPosLightSpace.xyz / objPosLightSpace.w);
            float realDepth = shadowProjection.z;
            float shadowFactor = casters[caster_i].ShadowStrength / (casters[caster_i].PcfPassNum * casters[caster_i].PcfPassNum);
            vec2 start_offset = (shadowProjection.xy / 2) + 0.5;
            for(int x = 0; x < casters[caster_i].PcfPassNum; x ++){
                for(int y = 0; y < casters[caster_i].PcfPassNum; y ++){
                    vec2 offset = vec2(x, y) / casters[caster_i].ShadowmapSize;
            
                    vec2 uvoffset = start_offset + offset;
                    float shadowmap_depth = GetShadowmapSample(caster_i, cascade, uvoffset);
                    if(realDepth <= 1.0) 
                        result += (realDepth - casters[caster_i].ShadowBias > shadowmap_depth) ? shadowFactor : 0.0;
                }
            }
        }
        if(casters[caster_i].caster_type == 1){
            vec3 dir = FragPos - casters[caster_i].pos;
            uint texture_index = GetPointShadowTextureIndex(caster_i);
            float shadowmap_depth = texture(shadowmaps_point[texture_index], dir).r;
            float real_depth = length(dir);
            if(shadowmap_depth > 0.00001 && result < 0.01)
                result += (real_depth - casters[caster_i].ShadowBias * 10 >= shadowmap_depth) ? casters[caster_i].ShadowStrength : 0.0;
        }
    }

    tColor = vec4(result, 0, 0, 0);
}