#pragma once

#include <string>
#include <Core/VarTypes/Base.hpp>
#include "ProjectSettings.hpp"

namespace VSGEditor {
	class Project {
	private:
		std::string m_rootDirectory;
		std::string m_assetsDirectory;
		std::string project_cache_dir;
		std::string m_configFilePath;

		ProjectSettings m_settings;
	public:
		Project(){}

		Project(
			const std::string& root, 
			const std::string& assets_dir = "assets",
			const std::string& cache_dir = "cache",
			const std::string& cfgFileName = "config.json");

		const std::string& GetRootDirectory() const;
		const std::string& GetAssetsDirectory() const;
		const std::string& GetCacheDirectory() const;

		ProjectSettings& getSettings();
		void saveSettings();
	};
}