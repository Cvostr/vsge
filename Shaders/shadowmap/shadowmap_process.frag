layout(location = 0) out vec4 tColor;

layout(location = 0) in vec2 UVCoord;

struct CasterInfo{  //672
    mat4 projections[10];
    //640
    float ShadowBias; //4
    int ShadowmapSize; //4
    uint CascadesNum; //4
    uint PcfPassNum; // 4
    float ShadowStrength; // 4
    vec3 pos;
};

layout (std140, binding = 0) uniform ShadowData{
    CasterInfo casters[64];
    uint casters_count;
};

void main(){
  tColor = vec4(1, 1, 1, 1);
}