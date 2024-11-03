#pragma once

#include <string>
#include <optional>

namespace VSGE {

	class ArgsParser {
	private:
		std::optional<std::string> _app_dir_path;
		int _graphics_api;
		bool _headless;
		bool _server;
		int _port;
		bool _debugGraphics;
	public:
		ArgsParser(int argc, char** argv);

		bool IsServer();
		bool IsHeadless();
		const std::optional<std::string>& GetApplicationDirPath();
		int GetServerPort();
		bool IsDebugGraphics();
	};
}