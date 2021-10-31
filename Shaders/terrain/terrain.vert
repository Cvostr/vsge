#version 450
//Vertex layout
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec3 InNormal;
layout(location = 2) out vec2 UVCoord;
layout(location = 3) out mat3 TBN;

layout (std140, binding = 0) uniform CamMatrices{
    mat4 cam_view_projection;
    mat4 cam_view;
    mat4 cam_projection;
    vec3 cam_position;
};

layout (std140, binding = 1) uniform Transform{
    uniform mat4 obj_model[4];
};

void main() { 
    UVCoord = uv;

    mat4 object_transform = obj_model[gl_InstanceIndex];

    InNormal = normalize(vec3(object_transform * vec4(normal, 0)));
    vec3 TangentVec = normalize(vec3(object_transform * vec4(tangent, 0)));
	vec3 BiTangentVec = normalize(vec3(object_transform * vec4(bitangent, 0)));
	TBN = mat3(TangentVec, BiTangentVec, InNormal);

    vec4 ModelPos = object_transform * vec4(position, 1.0);
    FragPos = ModelPos.xyz;

    gl_Position = cam_view_projection * ModelPos;
}
