#version 450

layout(location = 0) out vec4 tColor;
layout(location = 0) in vec2 UVCoord;

layout(binding = 0) uniform sampler2D inputHDR;
layout(binding = 1) uniform sampler2D inputBloom;

void main() {
    vec4 hdr = texture(inputHDR, UVCoord);
    vec3 hdr_color = hdr.rgb;
    hdr_color += texture(inputBloom, UVCoord).rgb;

    hdr_color = hdr_color / (hdr_color + vec3(1.0));
    hdr_color = pow(hdr_color, vec3(1.0/2.2));

    tColor = vec4(hdr_color, hdr.a);
}