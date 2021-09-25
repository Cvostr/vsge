#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 30) out;

layout (location = 0) out vec3 FragPos;

layout (std140, set = 1, binding = 0) uniform ShadowData{
    mat4 projections[10];
    vec3 pos;
    int type;
};

layout (std140, set = 1, binding = 1) uniform CascadesDir{
    float distances[10];
    int cascades_count;
};

layout(push_constant) uniform PushConstants
{
	int first_cascade;
	int last_cascade;
} pushConstants;

void main() { 
	int _first_cascade = 0;
	int _cascades = cascades_count;

	if(type == 0){
		_first_cascade = pushConstants.first_cascade;
		_cascades = pushConstants.last_cascade;
	}else
	if(type == 1) _cascades = 6;
	else if(type == 2) _cascades = 1;

	for(int cascade = _first_cascade; cascade < _cascades; cascade++){
		gl_Layer = cascade;
		for (int i = 0; i < gl_in.length(); i++)
		{
			gl_Position = projections[cascade] * gl_in[i].gl_Position;
			FragPos = gl_in[i].gl_Position.xyz;
			EmitVertex();
		}
		EndPrimitive();
	}
} 