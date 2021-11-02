#version 450 core

layout(location = 0) in vec2 UVCoord;
layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D image;

void main(){
    FragColor = texture(image, UVCoord);
}