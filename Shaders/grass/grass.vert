#version 450

//Vertex layout
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

layout (location = 0) out vec3 FragPos;
layout (location = 1) out vec3 Normal;
layout (location = 2) out vec2 UVCoord;

layout (std140, set = 0, binding = 0) uniform CamMatrices{
    mat4 cam_view_projection;
    mat4 cam_view;
    mat4 cam_projection;
    vec3 cam_position;
};

layout (std140, set = 0, binding = 1) readonly buffer Transform {
    mat4 obj_transform;
};

layout (std140, set = 0, binding = 2) uniform GeneralEnv {
    // Time params
    float timeSec;
    float reserved1;
    float reserved2;
    float reserved3;

    // Wind settings
    float windSpeed;
    float windStrength;
    vec2 windDirection;
};

layout (std140, set = 2, binding = 0) readonly buffer GrassTransforms {
    mat4 transforms[];
};

void main()
{
    mat4 instanceMatrix = transforms[gl_InstanceIndex];
    
    // mask to apply wind. 
    // Root of grass should stay on place
    float windMask = 1 - uv.y; 
    
    vec3 worldInstancePos = vec3(instanceMatrix[3][0], instanceMatrix[3][1], instanceMatrix[3][2]);

    // wave generation
    float wave = sin(timeSec * windSpeed + worldInstancePos.x * 0.5 + worldInstancePos.z * 0.5);
    wave += sin(timeSec * windSpeed * 2.0 + worldInstancePos.y) * 0.3; 

    // Apply wave
    float finalMask = pow(windMask, 2.0); 

    // Calc vectors
    vec4 pos = obj_transform * instanceMatrix * vec4(position, 1.0);
    pos.x += windDirection.x * wave * windStrength * finalMask;
    pos.z += windDirection.y * wave * windStrength * finalMask;

    FragPos = pos.xyz;
    UVCoord = uv;
    Normal = normal;
	gl_Position = cam_view_projection * pos;
}