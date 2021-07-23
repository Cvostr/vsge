#version 450 core

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

layout(binding = 3) uniform sampler2DArray shadowmaps[64];

int GetCascade(float dist){
  /*if(dist < 40){
        return 0;
    }else if(dist < 100){
        return 1;
    }else if(dist < 160){
        return 2;
    }
    return 2;*/
    return 0;
}

float GetShadowmapSample(uint shadowmap, int cascade, vec2 uv){
    return texture(shadowmaps[shadowmap], vec3(uv, cascade)).r;
}

void main(){
  vec3 FragPos = texture(gpos, UVCoord).xyz;

  float dist = length(FragPos - cam_position);
  int cascade = GetCascade(dist);

    float result = 0;

  for(uint caster_i = 0; caster_i < casters_count; caster_i ++){
    vec4 objPosLightSpace = casters[caster_i].projections[cascade] * vec4(FragPos, 1);
    vec3 shadowProjection = (objPosLightSpace.xyz / objPosLightSpace.w) / 2.0 + 0.5;
    float realDepth = shadowProjection.z;
    float shadowFactor = casters[caster_i].ShadowStrength / (casters[caster_i].PcfPassNum * casters[caster_i].PcfPassNum);

    for(int x = 0; x < casters[caster_i].PcfPassNum; x ++){
        for(int y = 0; y < casters[caster_i].PcfPassNum; y ++){
            vec2 offset = vec2(x, y);

            offset.x /= casters[caster_i].ShadowmapSize;
            offset.y /= casters[caster_i].ShadowmapSize;

            vec2 uvoffset = shadowProjection.xy + offset;
            float shadowmap_depth = GetShadowmapSample(caster_i, cascade, uvoffset);
           
            result += (realDepth - casters[caster_i].ShadowBias > shadowmap_depth) ? shadowFactor : 0.0;
        }
    }

    if(realDepth > 1.0) 
        result = 0.0;
  }

  tColor = vec4(result, 0, 0, 0);
}