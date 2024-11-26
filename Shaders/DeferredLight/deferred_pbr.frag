#version 450

layout (location = 0) out vec4 tColor;

layout(location = 0) in vec2 UVCoord;

layout(binding = 3) uniform sampler2D color;
layout(binding = 4) uniform sampler2D normal;
layout(binding = 5) uniform sampler2D pos;
layout(binding = 6) uniform sampler2D material;
layout(binding = 7) uniform sampler2D shadows;
layout(binding = 8) uniform sampler2D depth;
layout(binding = 9) uniform sampler2D brdf_lut;
layout(binding = 10) uniform samplerCube env_cube;
layout(binding = 11) uniform samplerCube env_cube_irradiance;
layout(binding = 12) uniform sampler2D ssao_map;
layout(binding = 13) uniform samplerCube ibl_specular[12];
layout(binding = 14) uniform samplerCube ibl_irradiance[12];

#define LIGHTSOURCE_DIR 0
#define LIGHTSOURCE_POINT 1
#define LIGHTSOURCE_SPOT 2
#define PI 3.14159265
#define RADIANS PI / 180

struct Light {
    int type;           //0
    float intensity;    //4
    float range;        //8
    float spot_angle;   //12
    vec3 position;      //16
    vec3 direction;     //32
    vec3 color;         //48
    int casterIndex;    //60
};

struct LightProbe {
    vec3 pos;
    float radius;
};

layout (std140, set = 0, binding = 1) uniform CamMatrices {
    mat4 cam_view_projection;
    mat4 cam_view;
    mat4 cam_projection;
    vec3 cam_position;
};

layout (std140, set = 0, binding = 2) uniform Lights {
    vec3 ambient_light;
    int lights_count;
    Light lights[200];
};

layout (std140, set = 0, binding = 15) uniform LightProbes {
    int probes_count;
    int pad1;
    int pad2;
    int pad3;
    LightProbe probes[12];
};

/////////// SHADOWS  //////////////

struct CasterInfo{  //688
    mat4 projections[10];
    //640
    float ShadowBias; // 4
    uint PcfPassNum; // 4
    float ShadowStrength; // 4
    int caster_type; // 4
    vec3 pos;
    float range;
    uint image_index;
};

layout (std140, set = 1, binding = 0) uniform ShadowData{
    CasterInfo casters[70];
    uint casters_count;
};

layout(set = 1, binding = 3) uniform sampler2DArray shadowmaps[2];
layout(set = 1, binding = 4) uniform samplerCube shadowmaps_point[32];
layout(set = 1, binding = 5) uniform sampler2D shadowmaps_spot[32];

layout(set = 1, binding = 6) uniform dir_shadow_cascades{
    float distances[10];
    uint cascades_count;
    uint shadowmap_size;
};

int GetCascadeByDistance(float dist){
	for(int i = 0; i < int(cascades_count); ++i) {
		if(dist < distances[i]) {	
			return i;
		}
	}
    return -1;
}

int GetCascade(vec3 position)
{
    float dist = (cam_view_projection * vec4(position, 1)).z;
    return GetCascadeByDistance(dist);
}

float GetShadowmapSample(uint shadowmap, uint cascade, vec2 uv){
    return texture(shadowmaps[shadowmap], vec3(uv, cascade)).r; 
}

float GetSpotShadowmapSample(uint shadowmap, vec2 uv)
{
    return texture(shadowmaps_spot[shadowmap], uv).r; 
}

float calculateDirectionalShadow(vec3 FragPos, int caster_i)
{
    if (caster_i == -1)
        return 0;

    uint cascade = GetCascade(FragPos);
    if (cascade < 0)
        return 0;
            
    vec4 objPosLightSpace = casters[caster_i].projections[cascade] * vec4(FragPos, 1);
    vec3 shadowProjection = (objPosLightSpace.xyz / objPosLightSpace.w);
    float realDepth = shadowProjection.z;
    vec2 start_offset = (shadowProjection.xy / 2) + 0.5;
    float shadowFactor = casters[caster_i].ShadowStrength / (casters[caster_i].PcfPassNum * casters[caster_i].PcfPassNum);

    float shadow = 0;
    for (int x = 0; x < casters[caster_i].PcfPassNum; x ++)
    {
        for (int y = 0; y < casters[caster_i].PcfPassNum; y ++)
        {
            vec2 uvoffset = start_offset + (vec2(x, y) / shadowmap_size);
            float shadowmap_depth = GetShadowmapSample(casters[caster_i].image_index, cascade, uvoffset);
            if (realDepth <= 1.0) 
                shadow += (realDepth - casters[caster_i].ShadowBias > shadowmap_depth) ? shadowFactor : 0.0;
        }
    }

    return shadow;
}

float calculatePointShadow(vec3 FragPos, int caster_i)
{
    float shadow = 0;
    if (caster_i == -1)
        return shadow;

    vec3 dir = FragPos - casters[caster_i].pos;
    uint texture_index = casters[caster_i].image_index;
    float shadowmap_depth = texture(shadowmaps_point[texture_index], dir).r * casters[caster_i].range;
    float real_depth = length(dir);
    
    if (shadowmap_depth < casters[caster_i].range)
    {
        shadow = (real_depth - casters[caster_i].ShadowBias * 10 >= shadowmap_depth) ? casters[caster_i].ShadowStrength : 0.0;
    }

    return shadow;
}

float calculateSpotShadow(vec3 FragPos, int caster_i)
{
    float shadow = 0;
    if (caster_i == -1)
        return shadow;

    vec4 objPosLightSpace = casters[caster_i].projections[0] * vec4(FragPos, 1);
    vec3 shadowProjection = (objPosLightSpace.xyz / objPosLightSpace.w);
    float realDepth = shadowProjection.z;
    vec2 start_offset = (shadowProjection.xy / 2) + 0.5;
    float shadowFactor = casters[caster_i].ShadowStrength / (casters[caster_i].PcfPassNum * casters[caster_i].PcfPassNum);
    for (int x = 0; x < casters[caster_i].PcfPassNum; x ++)
    {
        for (int y = 0; y < casters[caster_i].PcfPassNum; y ++)
        {
            vec2 uvoffset = start_offset + (vec2(x, y) / shadowmap_size);
            float shadowmap_depth = GetSpotShadowmapSample(casters[caster_i].image_index, uvoffset);
            if (realDepth <= 1.0) 
                shadow += (realDepth - casters[caster_i].ShadowBias > shadowmap_depth) ? shadowFactor : 0.0;
        }
    }

    return shadow;
}

////////////////////////////////////////////////////////////////

vec3 CalculateLightning(vec3 color, vec3 normal, vec3 pos, float roughness, float metallic, vec3 F0);

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.0000001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 CalculateLight(vec3 L, float attenuation, vec3 cam_to_frag, vec3 light_color, vec3 albedo, vec3 normal, float roughness, float metallic, vec3 F0)
{
    vec3 H = normalize(cam_to_frag + L);
    vec3 radiance = light_color * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(normal, H, roughness);   
    float G   = GeometrySmith(normal, cam_to_frag, L, roughness);      
    vec3 F    = fresnelSchlick(clamp(dot(H, cam_to_frag), 0.0, 1.0), F0);
           
    vec3 numerator    = NDF * G * F; 
    float denominator = 4 * max(dot(normal, cam_to_frag), 0.0) * max(dot(normal, L), 0.0);
    vec3 specular = numerator / max(denominator, 0.001); // prevent divide by zero for NdotV=0.0 or NdotL=0.0

    // kS is equal to Fresnel
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  

    // scale light by NdotL
    float NdotL = max(dot(normal, L), 0.0);        

    // add to outgoing radiance Lo
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

vec3 CalculateIBL(vec3 pos, vec3 Normal, vec3 F0, vec3 Lo, float cosLo, float roughness, float metalness, vec3 albedo)
{
    vec3 Lr = 2.0 * cosLo * Normal - Lo; 

    float min_dist = 1000000;
    int probe_id = -1;
    for (int i = 0; i < probes_count; i ++) {
        LightProbe probe = probes[i];
        float dist = length(pos - probe.pos);
        if (dist <= probe.radius && dist < min_dist) {
            probe_id = i;
            min_dist = dist;
        }
    }

    vec3 irradiance = vec3(0);
    
    if (probe_id == -1) {
        irradiance = texture(env_cube_irradiance, Normal).rgb;
    } else {
        irradiance = texture(ibl_irradiance[probe_id], Normal).rgb;
    }
    
    vec3 F = fresnelSchlick(cosLo, F0);
    vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metalness);
    vec3 diffuseIBL = kd * albedo * irradiance;

    int specularTextureLevels = 0;
    vec3 specularIrradiance = vec3(0);
    if (probe_id == -1) {
        specularTextureLevels = textureQueryLevels(env_cube);
        specularIrradiance = textureLod(env_cube, Lr, roughness * specularTextureLevels).rgb;
    } else {
        specularTextureLevels = textureQueryLevels(ibl_specular[probe_id]);
        specularIrradiance = textureLod(ibl_specular[probe_id], Lr, roughness * specularTextureLevels).rgb;
    }

    vec2 specularBRDF = texture(brdf_lut, vec2(cosLo, roughness)).rg;
    vec3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;

    return diffuseIBL + specularIBL;
}

vec3 CalculateLightning(vec3 Lo, vec3 albedo, vec3 normal, vec3 pos, float roughness, float metallic, vec3 F0)
{
    vec3 result = vec3(0);
    //Calculate direction from camera to processing fragment
    //iterate over all lights and calculate them
    for(int light_i = 0; light_i < lights_count; light_i ++) {

        if (lights[light_i].type == LIGHTSOURCE_DIR)
        {
			vec3 L = lights[light_i].direction;
            float shadow = calculateDirectionalShadow(pos, lights[light_i].casterIndex);
            result += CalculateLight(L, 
                                    lights[light_i].intensity, 
                                    Lo,
                                    lights[light_i].color,
                                    albedo,
                                    normal,
                                    roughness,
                                    metallic,
                                    F0) * (1 - shadow);
        }
        if (lights[light_i].type == LIGHTSOURCE_POINT)
        {
            vec3 L = normalize(lights[light_i].position - pos);

            float distance = length(lights[light_i].position - pos);
            float attenuation = 1.0 / ( 1.0 + 1.0 / lights[light_i].range * distance + 1.0 / lights[light_i].range * distance * distance) * lights[light_i].intensity;
            float shadow = calculatePointShadow(pos, lights[light_i].casterIndex);

            result += CalculateLight(L, 
                                    attenuation, 
                                    Lo,
                                    lights[light_i].color,
                                    albedo,
                                    normal,
                                    roughness,
                                    metallic,
                                    F0) * (1 - shadow);
        }

        if (lights[light_i].type == LIGHTSOURCE_SPOT)
        {
            vec3 L = normalize(lights[light_i].position - pos);
            vec3 direction = lights[light_i].direction;

            float inner = cos(lights[light_i].spot_angle * RADIANS);
            float outer = cos((lights[light_i].spot_angle + 12) * RADIANS);
            
            float theta     = dot(L, normalize(-direction));
            float epsilon   = inner - outer;
            float intensity = clamp((theta - outer) / epsilon, 0.0, 1.0);  

            float shadow = calculateSpotShadow(pos, lights[light_i].casterIndex);

            result += CalculateLight(L, 
                                    intensity * lights[light_i].intensity, 
                                    Lo,
                                    lights[light_i].color,
                                    albedo,
                                    normal,
                                    roughness,
                                    metallic,
                                    F0) * (1 - shadow);
        }
    }

    return result;
}

void main() {
    float depth = texture(depth, UVCoord).r;
    vec4 diffuse = texture(color, UVCoord);

    if(depth == 1.0)
        discard;
    else
    {
        vec4 normal_map = texture(normal, UVCoord);
        vec3 normal = normal_map.rgb;
        vec3 pos = texture(pos, UVCoord).rgb;
        vec4 material = texture(material, UVCoord);
        //float shadow = texture(shadows, UVCoord).r;
        float ssao = texture(ssao_map, UVCoord).r;

        vec3 albedo = pow(diffuse.rgb, vec3(2.2));
        float roughness = material.r;
        float metallic = material.g;
        float ao = diffuse.a * ssao;

        vec3 F0 = vec3(0.04); 
        F0 = mix(F0, albedo, metallic);

        vec3 Lo = normalize(cam_position - pos);

        vec3 lightning = CalculateLightning(Lo, albedo, normal, pos, roughness, metallic, F0) * ao;
        vec3 color = vec3(0.03) * albedo + lightning;
        
        float cosLo = max(0.0, dot(normal, Lo));
        vec3 ambient = CalculateIBL(pos, normal, F0, Lo, cosLo, roughness, metallic, albedo);
        color += ambient * ambient_light;

        tColor = vec4(color, 1);
    }
}