#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

layout (location = 0) out vec3 FragPos;
layout (location = 1) out vec2 UVCoord; 

layout (std140, binding = 0) uniform CamMatrices{
    uniform mat4 cam_view_projection;
    uniform vec3 cam_position;
};

layout (std140, binding = 1) uniform Transform{
    uniform mat4 obj_model[1000];
};

void main(){
	UVCoord = uv;

	FragPos = (obj_model[gl_InstanceIndex] * vec4(position, 1.0)).xyz;
    gl_Position = cam_view_projection * vec4(FragPos, 1);
	
}