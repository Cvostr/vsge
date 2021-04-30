#version 450 core

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 norm;
layout(location = 0) out vec2 UVCoord;

layout (binding = 0) uniform CamMatrices{
    float div;
}uni;

void main() { 
    vec4 v4pos = vec4(pos * uni.div, 1.0);
    gl_Position = v4pos;
    UVCoord = uv;
}