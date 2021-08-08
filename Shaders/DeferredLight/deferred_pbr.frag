layout (location = 0) out vec4 tColor;

layout(location = 2) in vec2 UVCoord;

layout(binding = 3) uniform sampler2D color;
layout(binding = 4) uniform sampler2D normal;
layout(binding = 5) uniform sampler2D pos;
layout(binding = 6) uniform sampler2D material;
layout(binding = 7) uniform sampler2D shadows;

#define LIGHTSOURCE_DIR 0
#define LIGHTSOURCE_POINT 1
#define LIGHTSOURCE_SPOT 2
#define PI 3.14159265

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
    uniform mat4 cam_view_projection;
    uniform vec3 cam_position;
};

layout (std140, binding = 2) uniform Lights{
    int lights_count;
    Light lights[200];
};

vec3 CalculateLightning(vec3 color, vec3 normal, vec3 pos, float roughness, float metallic, vec3 F0);

void main() {
    vec4 diffuse = texture(color, UVCoord);
    vec3 normal = texture(normal, UVCoord).rgb;
    vec3 pos = texture(pos, UVCoord).rgb;
    vec4 material = texture(material, UVCoord);
    float shadow = texture(shadows, UVCoord).r;

    vec3 albedo = diffuse.rgb;
    float roughness = material.r;
    float metallic = material.g;
    float emission = material.b;
    float ao = material.a;

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 lightning = CalculateLightning(albedo, normal, pos, roughness, metallic, F0) * ao;
    vec3 color = albedo * (1 - shadow) + lightning;
    //color = color / (color + vec3(1.0));
    //color = pow(color, vec3(1.0/2.2)); 

    tColor = vec4(color, 1);
}

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

vec3 CalculateLightning(vec3 albedo, vec3 normal, vec3 pos, float roughness, float metallic, vec3 F0) {
    vec3 result = vec3(0);
    //Calculate direction from camera to processing fragment
    vec3 camToFragDirection = normalize(cam_position - pos);
    //iterate over all lights and calculate them
    for(int light_i = 0; light_i < lights_count; light_i ++) {

        if(lights[light_i].type == LIGHTSOURCE_DIR){
            vec3 direction = lights[light_i].direction;
            float cosine = max(dot(normal, direction), 0);

            result += cosine * lights[light_i].color;
        }
        if(lights[light_i].type == LIGHTSOURCE_POINT){
            vec3 L = normalize(lights[light_i].position - pos);
            vec3 H = normalize(camToFragDirection + L);

            float distance = length(lights[light_i].position - pos);
            float attenuation = 1.0 / ( 1.0 + 1.0 / lights[light_i].range * distance + 1.0 / lights[light_i].range * distance * distance) * lights[light_i].intensity;
            vec3 radiance = lights[light_i].color * attenuation;

            // Cook-Torrance BRDF
            float NDF = DistributionGGX(normal, H, roughness);   
            float G   = GeometrySmith(normal, camToFragDirection, L, roughness);      
            vec3 F    = fresnelSchlick(clamp(dot(H, camToFragDirection), 0.0, 1.0), F0);
           
            vec3 numerator    = NDF * G * F; 
            float denominator = 4 * max(dot(normal, camToFragDirection), 0.0) * max(dot(normal, L), 0.0);
            vec3 specular = numerator / max(denominator, 0.001); // prevent divide by zero for NdotV=0.0 or NdotL=0.0

            // kS is equal to Fresnel
            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= 1.0 - metallic;	  

            // scale light by NdotL
            float NdotL = max(dot(normal, L), 0.0);        

            // add to outgoing radiance Lo
            result += (kD * albedo / PI + specular) * radiance * NdotL;
        }
    }
    return result;
}