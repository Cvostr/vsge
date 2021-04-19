#pragma once

#include <Core/VarTypes/Base.hpp>
#include "VulkanShader.hpp"

namespace VSGE {
	bool CompileFromGLSL(const char* shader_data, ShaderStagesBits stage, byte** out, uint32& spv_size);
}