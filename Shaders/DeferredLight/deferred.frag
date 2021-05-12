layout (location = 0) out vec4 tColor;
layout (location = 1) out vec3 tNormal;
layout (location = 2) out vec3 tPos;

layout(location = 2) in vec2 UVCoord;


layout(binding = 3) uniform sampler2D color;
layout(binding = 4) uniform sampler2D normal;
layout(binding = 5) uniform sampler2D pos;

void main() {
    tColor = texture(color, UVCoord);
}   
