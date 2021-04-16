#pragma once

#include <Core/VarTypes/Base.hpp>

namespace VSGE {

	typedef unsigned long long ShaderTypesBits;

	#define VERT_SHADER 0x1
	#define FRAG_SHADER 0x2
	#define GEOM_SHADER 0x4
	#define COMP_SHADER 0x8
	#define TESSCTRL_SHADER 0x10
	#define TESSEVAL_SHADER 0x20

	class Shader {
	private:
		ShaderTypesBits mTypes;
	public:
		virtual void AddShader(byte* shaderData, uint32 shaderSize, ShaderTypesBits type) = 0;

		Shader() : mTypes(0) 
		{}
	};
}