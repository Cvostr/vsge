#include "ResourceBundle.hpp"
#include <Core/ByteSolver.hpp>
#include <fstream>
#include <Core/Logger.hpp>

using namespace VSGE;

void ResourceBundle::CreateFromFile(const std::string& file) {
	std::ifstream stream;

	stream.open(file, std::ios::binary | std::ios::ate);

	if (stream.fail()) {
		Logger::Log(LogType::LOG_TYPE_ERROR) << "Error loading bundle file " << file << "\n";
		return;
	}

	uint32 size = stream.tellg();
	byte* bundle = new byte[size];
	stream.read((char*)bundle, size);

	ByteSolver solver(bundle, size);
	uint32 entriesCount = 0;
	solver.Copy(entriesCount);

	stream.close();
}
BundleEntry* ResourceBundle::GetEntry(const std::string& name) {
	return nullptr;
}