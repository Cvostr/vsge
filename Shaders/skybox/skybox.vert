#version 450

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;

layout (location = 0) out vec2 _uv;
layout (location = 1) out vec3 _pos;

layout (std140, binding = 0) uniform CamMatrices{
    mat4 cam_view_projection;
    mat4 cam_view;
    mat4 cam_projection;
    vec3 cam_position;
};

layout (std140, binding = 1) uniform Transform{
    uniform mat4 obj_model[4];
};

void main()
{
    _uv = uv;
    _pos = pos;

    mat4 modified_view = cam_view;
    modified_view[3][0] = 0;
    modified_view[3][1] = 0;
    modified_view[3][2] = 0;
    modified_view[3][3] = 1;
    modified_view[0][3] = 0;
    modified_view[1][3] = 0;
    modified_view[2][3] = 0;


    gl_Position = cam_projection * modified_view * vec4(pos, 1.0);
}  
