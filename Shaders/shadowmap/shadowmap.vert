#version 450 core

//Vertex layout
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

//IDs of bones, that control this vertex
layout (location = 5) in ivec4 BoneIDs;
layout (location = 6) in ivec4 BoneIDs1;
layout (location = 7) in ivec4 BoneIDs2;
//Coefficient of bones, that control this vertex
layout (location = 8) in vec4 Weights;
layout (location = 9) in vec4 Weights1;
layout (location = 10) in vec4 Weights2;
//Amount of bones, that control this vertex
layout (location = 11) in uint bones;

layout (std140, set = 1, binding = 0) uniform ShadowData{
    mat4 projections[10];
};

layout (std140, binding = 1) uniform Transform{
    uniform mat4 obj_model[4];
};

layout (std140, set = 2, binding = 0) readonly buffer Animation{
    mat4 bone_transform[];
};

layout (location = 0) out VS_OUTPUT
{
	float depth;
	flat int instanceID;
} vsoutput;

mat4 getBoneTransform(){
    int _ids[12];
    float _weights[12];
    
    _ids[0] = BoneIDs.x;
    _ids[1] = BoneIDs.y;
    _ids[2] = BoneIDs.z;
    _ids[3] = BoneIDs.w;
    _ids[4] = BoneIDs1.x;
    _ids[5] = BoneIDs1.y;
    _ids[6] = BoneIDs1.z;
    _ids[7] = BoneIDs1.w;
    _ids[8] = BoneIDs2.x;
    _ids[9] = BoneIDs2.y;
    _ids[10] = BoneIDs2.z;
    _ids[11] = BoneIDs2.w;
    
    _weights[0] = Weights.x;
    _weights[1] = Weights.y;
    _weights[2] = Weights.z;
    _weights[3] = Weights.w;
    _weights[4] = Weights1.x;
    _weights[5] = Weights1.y;
    _weights[6] = Weights1.z;
    _weights[7] = Weights1.w;
    _weights[8] = Weights2.x;
    _weights[9] = Weights2.y;
    _weights[10] = Weights2.z;
    _weights[11] = Weights2.w;
    
    mat4 result = mat4(0.0);
    
    for(int i = 0; i < 12; i++){
        result += bone_transform[_ids[i]] * _weights[i];
   }
	
    return result;
}

void main(){
	mat4 bone_t = mat4(1.0);

	if(bones > 0)
		bone_t = getBoneTransform();

    vec4 pos = projections[gl_InstanceIndex] * obj_model[0] * bone_t * vec4(position, 1.0);

	gl_Position = pos;
	vsoutput.depth = pos.z;
	vsoutput.instanceID = gl_InstanceIndex;
}
