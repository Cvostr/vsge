#version 450 core

layout(location = 0) out vec4 outColor;
layout(location = 0) in vec2 UVCoord;
layout(binding = 2) uniform sampler2D diffuse;
void main() {
outColor = texture(diffuse, UVCoord);
}   
