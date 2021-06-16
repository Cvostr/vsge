#version 450 core

layout (location = 0) out vec4 tDiffuse;
layout (location = 4) out vec4 tMasks;

layout (location = 0) in vec3 uv;

layout(set = 1, binding = 1) uniform samplerCube skybox;

layout (std140, set = 1, binding = 0) uniform Skybox{
    vec3 tint_color;
	float Exposure;
};

void main()
{   
	vec3 color = texture(skybox, uv).rgb;
	color *= tint_color;
	color *= Exposure;

	tDiffuse.xyz = color;
	tMasks = vec4(0, 0, 0, 0);
}
