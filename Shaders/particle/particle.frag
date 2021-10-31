#version 450 core

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec3 FragPos;
layout (location = 1) in vec2 UVCoord;
layout (location = 2) in vec3 new_depth; 
//textures
layout(set = 1, binding = 1) uniform sampler2D diffuse;
layout(set = 3, binding = 8) uniform sampler2D depth;

layout (std140, set = 1, binding = 0) uniform ParticleMaterial{
    bool hasParticleTexture;
    vec3 tint_color;
};

void main(){
	vec2 uv = UVCoord;
	
    float old_depth = texture(depth, new_depth.xy * 0.5 + 0.5).r;
    if(old_depth <= new_depth.z)
        discard;

	vec4 sprite = vec4(1, 1, 1, 1);
    if(hasParticleTexture)
        sprite = texture(diffuse, UVCoord);

    vec3 color = sprite.rgb * tint_color;
    color = pow(color, vec3(2.2));
	
    FragColor = vec4(color, sprite.a);
}