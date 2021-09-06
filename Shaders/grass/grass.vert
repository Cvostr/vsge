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

layout (std140, binding = 0) uniform CamMatrices{
    mat4 cam_view_projection;
    mat4 cam_view;
    vec3 cam_position;
};

layout (std140, binding = 1) uniform Transform{
    mat4 obj_model[4];
};

layout (std140, set = 1, binding = 0) readonly buffer Transforms {
    mat4 transforms[];
};

void main(){
    vec4 pos = obj_model[0] * transforms[gl_InstanceIndex] * vec4(position, 1.0);
    FragPos = pos.xyz;
    UVCoord = uv;
    Normal = normal;
	gl_Position = cam_view_projection * pos;
}