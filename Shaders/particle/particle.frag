#version 450 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 tNormal;
layout (location = 2) out vec3 tPos;
layout (location = 3) out vec4 tSpec;

layout (location = 0) in vec3 FragPos;
layout (location = 1) in vec2 UVCoord;
//textures
layout(set = 1, binding = 1) uniform sampler2D diffuse;

layout (std140, set = 1, binding = 0) uniform ParticleMaterial{
    bool hasParticleTexture;
    vec3 tint_color;
};

void main(){

	vec2 uv = UVCoord;
	
	vec4 sprite = vec4(1, 1, 1, 1);
    if(hasParticleTexture)
        sprite = texture(diffuse, UVCoord);

    vec3 color = sprite.rgb * tint_color;

    if(sprite.a < 0.05)
        discard;
	
    FragColor = vec4(color, sprite.a);
    tPos = FragPos;
}