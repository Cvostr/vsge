#version 450 core

layout(location = 0) in vec2 _UV;
layout(location = 0) out vec4 FragColor;

layout (std140, binding = 1) uniform Ui{
    vec3 color;
    float alpha;
    vec2 uvStart;
    vec2 uvSize;
    int text;
};

layout(binding = 2) uniform sampler2D sprite_map;

void main(){
    vec2 UV = _UV * uvSize + uvStart;
    vec4 textured = texture(sprite_map, UV);
    if(text == 1)
        textured = vec4(textured.r, textured.r, textured.r, textured.r);

    vec4 color = textured * vec4(color, alpha);

    FragColor = color;
}