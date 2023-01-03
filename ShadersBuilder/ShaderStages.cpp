#include "ShaderStages.hpp"

ShaderStagesBits GetStageBit(std::string ext) {
	if (ext == ".vert")
		return SHADER_STAGE_VERTEX;
	else if (ext == ".frag")
		return SHADER_STAGE_FRAGMENT;
	else if (ext == ".geom")
		return SHADER_STAGE_GEOMETRY;
	else if (ext == ".comp")
		return SHADER_STAGE_COMPUTE;

	return SHADER_STAGE_NULL;
}

const std::string GetShaderStageString(ShaderStagesBits stage_bits) {
	switch (stage_bits) {
	case SHADER_STAGE_VERTEX:
		return "Vertex";
	case SHADER_STAGE_FRAGMENT:
		return "Fragment";
	case SHADER_STAGE_GEOMETRY:
		return "Geometry";
	case SHADER_STAGE_COMPUTE:
		return "Compute";
	}
	return "";
}