#version 450

//Vertex layout
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

layout (std140, binding = 1) uniform Transform{
    uniform mat4 obj_model[4];
};

void main(){
    vec4 pos = obj_model[0] * vec4(position, 1.0);

	gl_Position = pos;
}
