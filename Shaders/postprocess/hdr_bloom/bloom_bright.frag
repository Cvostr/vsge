#version 450

layout(location = 0) out vec4 tColor;

layout(location = 0) in vec2 UVCoord;

layout(binding = 0) uniform sampler2D inputHDR;

void main() {
    vec3 hdr = texture(inputHDR, UVCoord).rgb;
    float brightness = dot(hdr, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 2)
        tColor = vec4(hdr, 1);
    else
        tColor = vec4(0, 0, 0, 1);
}