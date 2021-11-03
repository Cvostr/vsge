#pragma once

#include <string>

namespace VSGE {

	class ArgsParser {
	private:
		std::string _app_dir_path;
		int _graphics_api;
		bool _headless;
		bool _server;
		int _port;
	public:
		ArgsParser(int argc, char** argv);

		bool IsServer();
		bool IsHeadless();
		const std::string& GetApplicationDirPath();
		int GetServerPort();
	};
}