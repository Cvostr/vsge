#version 450

//Vertex layout
layout (location = 0) in vec3 position;

layout (std140, binding = 1) uniform Transform{
    uniform mat4 obj_model[4];
};

layout (std140, set = 2, binding = 0) readonly buffer Animation{
    mat4 bone_transform[];
};

void main(){
    vec4 pos = obj_model[0] * vec4(position, 1.0);

	gl_Position = pos;
}
