#version 450

layout (location = 0) out vec4 tColor;

layout(location = 2) in vec2 UVCoord;

layout(binding = 3) uniform sampler2D color;
layout(binding = 4) uniform sampler2D normal;
layout(binding = 5) uniform sampler2D pos;
layout(binding = 6) uniform sampler2D material;
layout(binding = 7) uniform sampler2D shadows;
layout(binding = 8) uniform sampler2D depth;
layout(binding = 9) uniform sampler2D brdf_lut;
layout(binding = 10) uniform samplerCube env_cube;
layout(binding = 11) uniform samplerCube env_cube_irradiance;

#define LIGHTSOURCE_DIR 0
#define LIGHTSOURCE_POINT 1
#define LIGHTSOURCE_SPOT 2
#define PI 3.14159265
#define RADIANS PI / 180

struct Light{
    int type;           //0
    float intensity;    //4
    float range;        //8
    float spot_angle;   //12
    vec3 position;      //16
    vec3 direction;     //32
    vec3 color;         //48
};

layout (std140, binding = 1) uniform CamMatrices{
    mat4 cam_view_projection;
    mat4 cam_view;
    vec3 cam_position;
};

layout (std140, binding = 2) uniform Lights{
    int lights_count;
    Light lights[200];
};

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

vec3 CalculateLight(vec3 L, float attenuation, vec3 cam_to_frag, vec3 light_color, vec3 albedo, vec3 normal, float roughness, float metallic, vec3 F0){
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

vec3 CalculateIBL(vec3 Normal, vec3 F0, float cosLo, float roughness, float metalness, vec3 albedo){
    Normal.y *= -1;
    vec3 irradiance = texture(env_cube, Normal).rgb;
    return irradiance * metalness;
    /*vec3 F = fresnelSchlick(cosLo, F0);
    vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metalness);
    vec3 diffuseIBL = kd * albedo * irradiance;

    vec2 specularBRDF = texture(brdf_lut, vec2(cosLo, roughness)).rg;


	vec3 specularIrradiance = diffuseIBL;
    // Total specular IBL contribution.
	vec3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;


    return diffuseIBL + specularIBL;*/
}

vec3 CalculateLightning(vec3 albedo, vec3 normal, vec3 pos, float roughness, float metallic, vec3 F0) {
    vec3 result = vec3(0);
    //Calculate direction from camera to processing fragment
    vec3 camToFragDirection = normalize(cam_position - pos);
    //iterate over all lights and calculate them
    for(int light_i = 0; light_i < lights_count; light_i ++) {

        if(lights[light_i].type == LIGHTSOURCE_DIR){
			vec3 L = lights[light_i].direction;
            result += CalculateLight(L, 
                                    lights[light_i].intensity, 
                                    camToFragDirection,
                                    lights[light_i].color,
                                    albedo,
                                    normal,
                                    roughness,
                                    metallic,
                                    F0);
        }
        if(lights[light_i].type == LIGHTSOURCE_POINT){
            vec3 L = normalize(lights[light_i].position - pos);

            float distance = length(lights[light_i].position - pos);
            float attenuation = 1.0 / ( 1.0 + 1.0 / lights[light_i].range * distance + 1.0 / lights[light_i].range * distance * distance) * lights[light_i].intensity;
            
            result += CalculateLight(L, 
                                    attenuation, 
                                    camToFragDirection,
                                    lights[light_i].color,
                                    albedo,
                                    normal,
                                    roughness,
                                    metallic,
                                    F0);
        }

        if(lights[light_i].type == LIGHTSOURCE_SPOT){
            vec3 L = normalize(lights[light_i].position - pos);
            vec3 direction = lights[light_i].direction;

            float inner = cos(lights[light_i].spot_angle * RADIANS);
            float outer = cos((lights[light_i].spot_angle + 12) * RADIANS);
            
            float theta     = dot(L, normalize(-direction));
            float epsilon   = inner - outer;
            float intensity = clamp((theta - outer) / epsilon, 0.0, 1.0);  

            result += CalculateLight(L, 
                                    intensity * lights[light_i].intensity, 
                                    camToFragDirection,
                                    lights[light_i].color,
                                    albedo,
                                    normal,
                                    roughness,
                                    metallic,
                                    F0);
        }
    }

    return result;
}

void main() {
    float depth = texture(depth, UVCoord).r;
    vec4 diffuse = texture(color, UVCoord);

    if(depth == 1.0)
        tColor = vec4(diffuse.rgb, 1);
    else{
        vec3 normal = texture(normal, UVCoord).rgb;
        vec3 pos = texture(pos, UVCoord).rgb;
        vec4 material = texture(material, UVCoord);
        float shadow = texture(shadows, UVCoord).r;

        vec3 albedo = pow(diffuse.rgb, vec3(2.2));
        float roughness = material.r;
        float metallic = material.g;
        float emission = material.b;
        float ao = material.a;

        vec3 F0 = vec3(0.04); 
        F0 = mix(F0, albedo, metallic);

        vec3 lightning = CalculateLightning(albedo, normal, pos, roughness, metallic, F0) * ao;
        vec3 color = vec3(0.03) * albedo + lightning * (1 - shadow);
        
        vec3 Lo = normalize(cam_position - pos);
        float cosLo = max(0.0, dot(normal, Lo));
        vec3 ambient = CalculateIBL(normal, F0, cosLo, roughness, metallic, albedo);
        color += ambient;

        color = color / (color + vec3(1.0));
        color = pow(color, vec3(1.0/2.2)); 

        tColor = vec4(color, 1);
    }
}