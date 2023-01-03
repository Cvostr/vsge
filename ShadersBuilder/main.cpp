#include <iostream>
#include <vector>
#include "io/FileLoader.hpp"
#include "compilers/glsl_compiler.hpp"
#include "io/ShaderBundleWriter.hpp"
#include "ShaderStages.hpp"

using namespace std;

int main(int argc, char** argv) {
	cout << "VSGE shaders builder v1.0 " << endl;

	std::string outputBundleFile;
	std::string outputMapFile;
	std::vector<std::string> inputShaderFiles; 

	for (int arg_i = 1; arg_i < argc; arg_i++) {
		std::string arg_stdstr = std::string(argv[arg_i]);
		if(arg_stdstr == "-ob") {
			outputBundleFile = std::string(argv[++arg_i]);
		} else if(arg_stdstr == "-om") {
			outputMapFile = std::string(argv[++arg_i]);
		} else {
			std::string filePath = std::string(argv[arg_i]);
			inputShaderFiles.push_back(filePath);
		}
	}

	ShaderBundleWriter writer;
	if (!writer.Create(outputBundleFile, outputMapFile)) {
		std::cout << "Error creating destination files!" << std::endl << "Exiting..." << std::endl;
		return 2;
	}

	cout << "Shaders to compile " << inputShaderFiles.size() << endl;

	for (std::string shader : inputShaderFiles) {
		cout << "Compiling shader " << shader << endl;

		char* data = nullptr;
		unsigned int size = 0;

		bool result = LoadFile(shader, &data, &size, true);
		if (!result) {
			cout << "Error loading shader file " << shader << endl;
			continue;
		}

		int lastDotPos = shader.find_last_of('.');
		std::string shaderFileExt = shader.substr(lastDotPos);
		ShaderStagesBits shader_type = GetStageBit(shaderFileExt);
		cout << "\t Shader type : " << GetShaderStageString(shader_type) << endl;
		unsigned char* out = nullptr;
		unsigned int out_size;
		result = CompileFromGLSL(data, shader_type, &out, out_size);

		if (!result) {
			cout << "Error compiling shader, skipping" << endl;
			continue;
		}

		cout << "\t Output size " << out_size << " bytes" << endl;

		std::string shaderName = shader;
		size_t shaderPathSlashPos = shader.find_last_of('/');
		if(shaderPathSlashPos != std::string::npos) {
			shaderName = shader.substr(shaderPathSlashPos + 1);
		}
		writer.WriteShader(out, out_size, shaderName);
	}
	writer.Close();
	
}