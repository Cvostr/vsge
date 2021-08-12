#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

layout (location = 0) out vec3 FragPos;
layout (location = 1) out vec2 UVCoord; 

layout (std140, binding = 0) uniform CamMatrices{
    mat4 cam_view_projection;
    mat4 cam_view;
    vec3 cam_position;
};

layout (std140, set = 2, binding = 0) readonly buffer Transforms {
    mat4 transforms[];
};

void main(){
	UVCoord = uv;

	FragPos = (transforms[gl_InstanceIndex] * vec4(position, 1.0)).xyz;
    gl_Position = cam_view_projection * vec4(FragPos, 1);	
}
