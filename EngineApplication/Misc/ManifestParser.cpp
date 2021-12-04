#include "ManifestParser.hpp"
#include <fstream>
#include <Core/VarTypes/Base.hpp>
#include <Core/ByteSolver.hpp>

using namespace VSGE;

ManifestParser::ManifestParser() {

}

bool ManifestParser::ParseFromFile(const std::string& manifest_file_path) {
	std::ifstream stream(manifest_file_path, std::ios_base::binary | std::ios_base::ate);
	if (stream.fail()) {
		return false;
	}

	uint32 size = static_cast<uint32>(stream.tellg());
	stream.seekg(0, std::ios::beg);
	byte* data = new byte[size];
	stream.read((char*)data, size);
	stream.close();

	ByteSolver solver(data, size);
	_project_name = solver.ReadNextString();
	_main_scene = solver.ReadNextString();
}

const std::string& ManifestParser::GetProjectName() {
	return _project_name;
}

const std::string& ManifestParser::GetMainScene() {
	return _main_scene;
}