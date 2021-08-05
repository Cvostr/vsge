#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 30) out;

layout (location = 0) out vec3 FragPos;

layout (std140, set = 1, binding = 0) uniform ShadowData{
    mat4 projections[10];
    vec3 pos;
    int type;
    int cascades;
};

void main() { 
	for( int cascade = 0; cascade < cascades; cascade++){
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