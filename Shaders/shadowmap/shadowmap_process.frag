#version 450

layout(location = 0) out vec4 tColor;

layout(location = 0) in vec2 UVCoord;

struct CasterInfo{  //688
    mat4 projections[10];
    //640
    float ShadowBias; //4
    uint PcfPassNum; // 4
    float ShadowStrength; // 4
    int caster_type; //4
    vec3 pos;
    float range;
    uint caster_index;
};

layout (std140, binding = 0) uniform ShadowData{
    CasterInfo casters[64];
    uint casters_count;
};

layout(binding = 1) uniform sampler2D gpos;

layout (std140, binding = 2) uniform CamMatrices{
    mat4 cam_view_projection;
    mat4 cam_view;
    vec3 cam_position;
};

layout(binding = 3) uniform sampler2DArray shadowmaps[2];
layout(binding = 4) uniform samplerCube shadowmaps_point[32];
layout(binding = 5) uniform sampler2D shadowmaps_spot[32];


layout(binding = 6) uniform dir_shadow_cascades{
    float distances[10];
    uint cascades_count;
    uint shadowmap_size;
};

uint GetCascadeByDistance(float dist){
	for(uint i = 0; i < cascades_count; ++i) {
		if(dist < distances[i]) {	
			return i;
		}
	}
    return 0;
}

uint GetCascade(vec3 position){
    float dist = (cam_view_projection * vec4(position, 1)).z;
    return GetCascadeByDistance(dist);
}

float GetShadowmapSample(uint shadowmap, uint cascade, vec2 uv){
    return texture(shadowmaps[shadowmap], vec3(uv, cascade)).r; 
}

float GetSpotShadowmapSample(uint shadowmap, vec2 uv){
    return texture(shadowmaps_spot[shadowmap], uv).r; 
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
            uint cascade = GetCascade(FragPos);
            
            vec4 objPosLightSpace = casters[caster_i].projections[cascade] * vec4(FragPos, 1);
            vec3 shadowProjection = (objPosLightSpace.xyz / objPosLightSpace.w);
            float realDepth = shadowProjection.z;
            vec2 start_offset = (shadowProjection.xy / 2) + 0.5;
            float shadowFactor = casters[caster_i].ShadowStrength / (casters[caster_i].PcfPassNum * casters[caster_i].PcfPassNum);

            float shadow = 0;
            for(int x = 0; x < casters[caster_i].PcfPassNum; x ++){
                for(int y = 0; y < casters[caster_i].PcfPassNum; y ++){
                    vec2 uvoffset = start_offset + (vec2(x, y) / shadowmap_size);
                    float shadowmap_depth = GetShadowmapSample(casters[caster_i].caster_index, cascade, uvoffset);
                    if(realDepth <= 1.0) 
                        shadow += (realDepth - casters[caster_i].ShadowBias > shadowmap_depth) ? shadowFactor : 0.0;
                }
            }

            if(result < 0.01)
                result += shadow;
            else if(shadow > 0.01) {
                result = casters[caster_i].ShadowStrength;
            }
        }
        if(casters[caster_i].caster_type == 1){
            vec3 dir = FragPos - casters[caster_i].pos;
            uint texture_index = casters[caster_i].caster_index;
            float shadowmap_depth = texture(shadowmaps_point[texture_index], dir).r;
            float real_depth = length(dir);
            if(shadowmap_depth < casters[caster_i].range){
                float shadow = (real_depth - casters[caster_i].ShadowBias * 10 >= shadowmap_depth) ? casters[caster_i].ShadowStrength : 0.0;
                if(result < 0.01)
                    result += shadow;
                else if(shadow > 0.01) {
                    result = casters[caster_i].ShadowStrength;
                }
            }
        }
        if(casters[caster_i].caster_type == 2){
            vec4 objPosLightSpace = casters[caster_i].projections[0] * vec4(FragPos, 1);
            vec3 shadowProjection = (objPosLightSpace.xyz / objPosLightSpace.w);
            float realDepth = shadowProjection.z;
            vec2 start_offset = (shadowProjection.xy / 2) + 0.5;
            float shadowFactor = casters[caster_i].ShadowStrength;
            float shadowmap_depth = GetSpotShadowmapSample(casters[caster_i].caster_index, start_offset);
            if(realDepth <= 1.0 && shadowmap_depth < 1.0){
                float shadow = (realDepth - casters[caster_i].ShadowBias > shadowmap_depth) ? shadowFactor : 0.0;
                if(result < 0.01) 
                    result += shadow;
                else if(shadow > 0.01) {
                    result = casters[caster_i].ShadowStrength;
                }
            }
        }
    }

    tColor = vec4(result, 0, 0, 0);
}
