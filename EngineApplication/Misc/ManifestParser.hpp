#pragma once

#include <string>
#include <mpi/Core/Guid.hpp>

namespace VSGE {
	class ManifestParser {
	private:
		std::string _project_name;
		Guid _main_scene;
	public:

		ManifestParser();

		bool ParseFromFile(const std::string& manifest_file_path);

		const std::string& GetProjectName();

		const Guid& GetMainScene();
	};
}