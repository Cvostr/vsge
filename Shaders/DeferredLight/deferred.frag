layout (location = 0) out vec4 tColor;

layout(location = 2) in vec2 UVCoord;

layout(binding = 3) uniform sampler2D color;
layout(binding = 4) uniform sampler2D normal;
layout(binding = 5) uniform sampler2D pos;

void main() {
    tColor = texture(color, UVCoord);
}   
