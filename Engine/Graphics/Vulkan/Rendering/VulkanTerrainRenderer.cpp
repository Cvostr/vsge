#include "VulkanTerrainRenderer.hpp"

using namespace VSGE;

void VulkanTerrainRenderer::Create() {
	_terrain_shader = new VulkanShader;
	_terrain_shader->AddShaderFromFile("terrain.vert", SHADER_STAGE_VERTEX);
	_terrain_shader->AddShaderFromFile("terrain.frag", SHADER_STAGE_FRAGMENT);
}