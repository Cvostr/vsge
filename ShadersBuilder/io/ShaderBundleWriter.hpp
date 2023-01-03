#pragma once

#include <fstream>
#include <string>

class ShaderBundleWriter {
private:
	std::ofstream stream;
	std::ofstream map_stream;

	unsigned int written_bytes;

public:

	ShaderBundleWriter() {
		written_bytes = 0;
	}

	~ShaderBundleWriter() {

	}

	bool Create(std::string file_path, std::string map_file_path);
	void WriteShader(unsigned char* data, unsigned int size, std::string name);
	void Close();
};