#version 450

layout(location = 0) in vec3 FragPos;

layout (std140, set = 1, binding = 0) uniform ShadowData{
    mat4 projections[10];
    vec3 pos;
    int type;
    int cascades;
};

void main()
{
    float lightDistance = length(FragPos - pos);
    gl_FragDepth = lightDistance / 100.f;
}