#version 450

layout(location = 0) in vec2 UVCoord;
layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D image;
layout(binding = 1) uniform sampler2D ui;

void main(){
    vec3 scene = texture(image, UVCoord).rgb;
    vec4 ui_sampled = texture(ui, UVCoord);

    vec3 color_result = mix(scene, ui_sampled.rgb, ui_sampled.a);

    FragColor = vec4(color_result, 1);
}