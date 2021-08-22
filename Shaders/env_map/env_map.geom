#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices = 30) out;

layout (location = 0) out vec3 FragPos;

layout (std140, binding = 0) uniform CamMatrices{
    mat4 view_projections[6];
};

void main() { 
	for( int cascade = 0; cascade < 6; cascade++){
		gl_Layer = cascade;
		for (int i = 0; i < gl_in.length(); i++)
		{
			gl_Position = view_projections[cascade] * gl_in[i].gl_Position;
			FragPos = gl_in[i].gl_Position.xyz;
			EmitVertex();
		}
		EndPrimitive();
	}
} 