#version 450

layout(location = 0) in vec2 UVCoord;
layout(location = 0) out vec4 tColor;

layout(binding = 0) uniform sampler2D inputHDR;

float strength = 3;

const float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {
    vec2 tex_offset = 1.0 / textureSize(inputHDR, 0);
    vec3 result = texture(inputHDR, UVCoord).rgb * weight[0]; 
    for(int i = 1; i < 5; ++i)
    {
        result += texture(inputHDR, UVCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i] * strength;
        result += texture(inputHDR, UVCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i] * strength;
    }
    tColor = vec4(result, 1);
}