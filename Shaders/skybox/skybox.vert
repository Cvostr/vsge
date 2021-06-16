#version 450 core

layout (location = 0) in vec3 aPos;

layout (location = 0) out vec3 uv;

layout (std140, binding = 6) uniform Skybox{
	uniform mat4 projection;
	uniform mat4 view;
};

void main()
{
    uv = aPos;
    gl_Position = projection * view * vec4(aPos, 1.0);
}  
