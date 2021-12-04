#pragma once

#include <string>

namespace VSGE {
	class ManifestParser {
	private:
		std::string _project_name;
		std::string _main_scene;
	public:

		ManifestParser();

		bool ParseFromFile(const std::string& manifest_file_path);

		const std::string& GetProjectName();

		const std::string& GetMainScene();
	};
}