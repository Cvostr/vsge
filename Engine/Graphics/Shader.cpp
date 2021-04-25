#include "Shader.hpp"
#include <Core/FileLoader.hpp>

using namespace VSGE;

void Shader::AddShaderFromFile(std::string file_path, ShaderStagesBits type) {
	char* data = nullptr;
	uint32 size = 0;
	LoadFile(file_path, &data, &size);

	AddShader((byte*)data, size, type);
}