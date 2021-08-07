#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;

layout (location = 0) out vec3 _uv;

layout (std140, binding = 0) uniform CamMatrices{
    uniform mat4 cam_view_projection;
    uniform vec3 cam_position;
};

layout (std140, binding = 1) uniform Transform{
    uniform mat4 obj_model[4];
};

void main()
{
    _uv = pos;
    gl_Position = cam_view_projection * vec4(pos, 1.0);
}  
