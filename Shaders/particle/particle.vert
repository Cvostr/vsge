#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

layout (location = 0) out vec3 FragPos;
layout (location = 1) out vec2 UVCoord;
layout (location = 2) out vec3 new_depth; 

layout (std140, binding = 0) uniform CamMatrices{
    mat4 cam_view_projection;
    mat4 cam_view;
    mat4 cam_projection;
    vec3 cam_position;
};

layout (std140, set = 2, binding = 0) readonly buffer Transforms {
    mat4 transforms[];
};

mat4 RemoveRotationFromTransform(mat4 transform, mat4 camera_view) {
    mat4 result = transform;

    result[0][0] = camera_view[0][0];
    result[0][1] = camera_view[1][0];
    result[0][2] = camera_view[2][0];

    result[1][0] = camera_view[0][1];
    result[1][1] = camera_view[1][1];
    result[1][2] = camera_view[2][1];

    result[2][0] = camera_view[0][2];
    result[2][1] = camera_view[1][2];
    result[2][2] = camera_view[2][2];

    return result;
}

mat4 GetTranslationMatrix(vec3 pos) {
    mat4 mat;

    mat[0][0] = 1; mat[0][1] = 0; mat[0][2] = 0; mat[0][3] = 0;
    mat[1][0] = 0; mat[1][1] = 1; mat[1][2] = 0; mat[1][3] = 0;
    mat[2][0] = 0; mat[2][1] = 0; mat[2][2] = 1; mat[2][3] = 0;
    mat[3][0] = pos.x; mat[3][1] = pos.y; mat[3][2] = pos.z; mat[3][3] = 1;

    return mat;
}

void main(){
	UVCoord = uv;

    mat4 transform = transforms[gl_InstanceIndex];
    /*vec3 translation = vec3(transform[3][0], transform[3][1], transform[3][2]);
    transform[3][0] = 0;
    transform[3][1] = 0;
    transform[3][2] = 0;

    mat4 transformMat = GetTranslationMatrix(translation);
    mat4 removed = RemoveRotationFromTransform(transformMat, cam_view);
    mat4 result = transform * removed;*/

	FragPos = (transform * vec4(position, 1.0)).xyz;
    gl_Position = cam_view_projection * vec4(FragPos, 1);
    new_depth = gl_Position.xyz / gl_Position.w;
}
