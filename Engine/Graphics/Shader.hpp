#pragma once

#include <Core/VarTypes/Base.hpp>
#include "GpuObject.hpp"
#include <string>
#include <vector>

namespace VSGE {

	class ShaderStorageItem {
	public:
		std::string name;

		byte* shaderData;
		unsigned int shaderDataSize;

		ShaderStorageItem() :
			shaderData(nullptr),
			shaderDataSize(0)
		{}
	};

	class ShaderStorage {
	private:

		static ShaderStorage* _this;

		std::vector<ShaderStorageItem> shaders;

	public:
		ShaderStorage() 
		{
			_this = this;
		}

		static ShaderStorage* Get() {
			return _this;
		}

		ShaderStorageItem* GetItemByName(std::string& name);

		void LoadShaderBundle(const std::string& bundle_path, const std::string& map_path);
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

	class Shader : public IGpuObject {
	protected:
		ShaderStagesBits mStages;
	public:
		virtual void AddShader(byte* shaderData, uint32 shaderSize, ShaderStagesBits type) = 0;
		void AddShaderFromFile(std::string file_path, ShaderStagesBits type);
		virtual void Destroy() = 0;

		Shader() : mStages(SHADER_STAGE_NULL)
		{}
	};
}