layout (location = 0) out vec4 tColor;
layout (location = 1) out vec3 tNormal;
layout (location = 2) out vec3 tPos;

layout(location = 2) in vec2 UVCoord;


layout(binding = 3) uniform sampler2D color;
layout(binding = 4) uniform sampler2D normal;
layout(binding = 5) uniform sampler2D pos;
//layout(set = 1, binding = 1) uniform sampler2D normal;

void main() {
  //  tColor = texture(diffuse, UVCoord);
    //vec3 normal = texture(normal, UVCoord).rgb;
    tColor = vec4(1,1,1,1);
}   
