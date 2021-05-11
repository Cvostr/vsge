#extension GL_ARB_separate_shader_objects : enable
//Vertex layout
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

layout(location = 2) out vec2 UVCoord;

void main() { 
    UVCoord = uv;

    gl_Position = vec4(position, 1.0);
}