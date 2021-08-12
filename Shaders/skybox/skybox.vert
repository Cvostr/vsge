#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;

layout (location = 0) out vec2 _uv;
layout (location = 1) out vec3 _pos;

layout (std140, binding = 0) uniform CamMatrices{
    mat4 cam_view_projection;
    mat4 cam_view_projection_skybox;
    vec3 cam_position;
};

layout (std140, binding = 1) uniform Transform{
    uniform mat4 obj_model[4];
};

void main()
{
    _uv = uv;
    _pos = pos;
    gl_Position = cam_view_projection_skybox * vec4(pos, 1.0);
}  
