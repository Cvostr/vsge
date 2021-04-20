#pragma once

#include <Core/VarTypes/Base.hpp>
#include "GpuObject.hpp"

namespace VSGE {

	enum ShaderStagesBits {
		SHADER_STAGE_NULL = 0x0,
		SHADER_STAGE_VERTEX = 0x1,
		SHADER_STAGE_FRAGMENT = 0x2,
		SHADER_STAGE_GEOMETRY = 0x4,
		SHADER_STAGE_COMPUTE = 0x8,
		SHADER_STAGE_TESSELATION_CTRL = 0x10,
		SHADER_STAGE_TESSELATION_EVAL = 0x20
	};

	class Shader : public IGpuObject {
	protected:
		ShaderStagesBits mStages;
	public:
		virtual void AddShader(byte* shaderData, uint32 shaderSize, ShaderStagesBits type) = 0;
		virtual void Destroy() = 0;

		Shader() : mStages(SHADER_STAGE_NULL)
		{}
	};
}