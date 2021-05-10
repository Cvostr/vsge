#include "Shader.hpp"
#include <Core/FileLoader.hpp>
#include <fstream>
#include <Core/ByteSolver.hpp>
#include <Core/Logger.hpp>

using namespace VSGE;

ShaderStorage* ShaderStorage::_this = nullptr;
ShaderCache* ShaderCache::_this = nullptr;

ShaderStorage storage;
ShaderCache shaderCache;

void Shader::AddShaderFromFile(std::string file_path, ShaderStagesBits type) {
	ShaderStorageItem* item = ShaderStorage::Get()->GetItemByName(file_path);

	byte* data = nullptr;
	uint32 size = 0;

	if (item == nullptr) {
		LoadFile(file_path, (char**)&data, &size);
	}
	else {
		data = item->shaderData;
		size = item->shaderDataSize;
	}

	AddShader(data, size, type);
}

ShaderStorageItem* ShaderStorage::GetItemByName(std::string& name) {
	for (ShaderStorageItem& shader : shaders) {
		if (shader.name == name)
			return &shader;
	}
	return nullptr;
}

void ShaderStorage::LoadShaderBundle(const std::string& bundle_path, const std::string& map_path) {
	std::ifstream bundle_stream;
	std::ifstream map_stream;

	map_stream.open(map_path);

	if (map_stream.fail())
	{
		Logger::Log(LogType::LOG_TYPE_ERROR) << "Error openging shader map file " << map_path << '\n';
		return;
	}

	bundle_stream.open(bundle_path, std::ifstream::binary);

	map_stream.seekg(0, std::ios::end); 
	uint32 map_size = static_cast<uint32>(map_stream.tellg());  
	map_stream.seekg(0, std::ios::beg);
	byte* data = new byte[map_size];
	map_stream.read((char*)data, map_size);

	ByteSolver solver(data, map_size);

	while (!solver.end()) {
		ShaderStorageItem item;

		item.name = solver.ReadNextString();
		uint32 offset = 0;
		solver.Copy(offset);
		solver.Copy(item.shaderDataSize);
		item.shaderData = new byte[item.shaderDataSize];
		bundle_stream.read((char*)item.shaderData, item.shaderDataSize);

		this->shaders.push_back(item);
	}
}

void ShaderCache::AddShader(Shader* shader, const std::string& shader_name) {
	ShaderStringPair pair = ShaderStringPair(shader_name, shader);
	_shaders.push_back(pair);
}

Shader* ShaderCache::GetShader(const std::string& name) {
	for (ShaderStringPair shader : _shaders) {
		if (shader.first == name)
			return shader.second;
	}
	return nullptr;
}