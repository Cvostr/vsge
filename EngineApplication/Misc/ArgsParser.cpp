#include "ArgsParser.hpp"
#include <string.h>

using namespace VSGE;

ArgsParser::ArgsParser(int argc, char** argv) {
	_graphics_api = 0;
	_headless = false;
	_server = false;
	_port = 16273;

	for (int i = 1; i < argc; i++) {
		char* key = argv[i];
		if (strcmp(key, "-headless") == 0)
			_headless = true;
		else if (strcmp(key, "-server") == 0)
			_server = true;
		else if (strcmp(key, "-app_dir") == 0) {
			_app_dir_path = std::string(argv[++i]);
		}
	}
}

bool ArgsParser::IsServer() {
	return _server;
}
bool ArgsParser::IsHeadless() {
	return _headless;
}
const std::string& ArgsParser::GetApplicationDirPath() {
	return _app_dir_path;
}
int ArgsParser::GetServerPort() {
	return _port;
}