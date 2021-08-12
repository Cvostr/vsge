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

vec3 CalculateLightning(vec3 color, vec3 normal, vec3 pos);

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

    vec3 color = albedo * (1 - shadow);
    vec3 lightning = CalculateLightning(diffuse.rgb, normal, pos);
    color += lightning;

    tColor = vec4(color, 1);
}

vec3 CalculateLightning(vec3 color, vec3 normal, vec3 pos) {
    vec3 result = vec3(0);
    //Calculate direction from camera to processing fragment
    vec3 camToFragDirection = normalize(cam_position - pos);
    //iterate over all lights and calculate them
    for(int lg = 0; lg < lights_count; lg ++) {
        if(lights[lg].type == LIGHTSOURCE_DIR){
            float lightcoeff = max(dot(normal, normalize(lights[lg].direction)), 0.0) * lights[lg].intensity;
            vec3 rlight = lightcoeff * lights[lg].color;
            //Specular calculation
            vec3 lightDirReflected = reflect(normalize(-lights[lg].direction), normal);
		    //add light to result color
            result += rlight;
        }
        if(lights[lg].type == LIGHTSOURCE_POINT){
            //distance from light to fragment
            float Dist = length(lights[lg].position - pos);
            vec3 Dir = normalize(lights[lg].position - pos);
            //calculate factor of light
            float factor = 1.0 / ( 1.0 + 1.0 / lights[lg].range * Dist + 1.0 / lights[lg].range * Dist * Dist) * lights[lg].intensity;
            
            float lightcoeff = max(dot(normal, Dir), 0.0) * lights[lg].intensity;
            vec3 rlight = lightcoeff * lights[lg].color;
            //Specular calculation
            vec3 lightDirReflected = reflect(-Dir, normal);
		    //add light to result color
            result += rlight * factor;
        }
        if(lights[lg].type == LIGHTSOURCE_SPOT){
            
            vec3 vec_dir = normalize(lights[lg].direction);
            vec3 vec_frag_light = (lights[lg].position - pos);
            
            float vangle = dot(vec_dir, normalize(vec_frag_light));
            
            float spot_angle = cos(lights[lg].spot_angle);
            float spot_out_angle = cos(spot_angle + 0.2);
            
            float epsilon   = spot_angle - spot_out_angle;
            float intensity = clamp((vangle - spot_out_angle) / epsilon, 0.0, 1.0);
            intensity = intensity;
            result += lights[lg].color * intensity * lights[lg].intensity * (lights[lg].range / length(vec_frag_light));
            
        }
    }
    return result;
}
