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
    uint cascades_count;
};


void main() { 

	uint _cascades = cascades_count;

	if(type == 1) _cascades = 6;

	for( int cascade = 0; cascade < _cascades; cascade++){
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