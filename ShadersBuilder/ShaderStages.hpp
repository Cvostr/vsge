#pragma once

#include <string>

enum ShaderAPI {
	API_VULKAN,
	API_D3D12
};

enum ShaderStagesBits {
	SHADER_STAGE_NULL = 0x0,
	SHADER_STAGE_VERTEX = 0x1,
	SHADER_STAGE_FRAGMENT = 0x2,
	SHADER_STAGE_GEOMETRY = 0x4,
	SHADER_STAGE_COMPUTE = 0x8,
	SHADER_STAGE_TESSELATION_CTRL = 0x10,
	SHADER_STAGE_TESSELATION_EVAL = 0x20
};

ShaderStagesBits GetStageBit(std::string ext);
const std::string GetShaderStageString(ShaderStagesBits stage_bits);