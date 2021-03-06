#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;

layout(location = 0) out vec2 _UV;

layout (std140, binding = 0) uniform Transform{
    mat4 transform;
};

void main(){
    _UV = uv;
    gl_Position = transform * vec4(pos, 1);
}
