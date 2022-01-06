#version 450

layout(location = 0) out vec4 tColor;

layout(location = 0) in vec2 UVCoord;

layout(set=0, binding=0) uniform sampler2D ssao_input;

void main() {
    vec2 size = textureSize(ssao_input, 0);
    float result = 0.0;

    for (int x = -2; x < 2; ++x) {
        for (int y = -2; y < 2; ++y) {
            vec2 offset = vec2(x, y) / size;
            vec4 color = texture(ssao_input, UVCoord + offset);
            result += color.r;
            if(x == 0 && y == 0 && color.a == 0){
                result = 0;
                break;
            }

        }
    }

    tColor = vec4(result / 16.0, 0, 0, 1);    
}