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
		std::string m_csprojDirectory;

		ProjectSettings m_settings;
	public:
		Project(){}

		Project(
			const std::string& root,
			const std::string& assetsDir = "assets",
			const std::string& cacheDir = "cache",
			const std::string& cfgFileName = "config.json",
			const std::string& csprojDir = "csproj");

		const std::string& GetName() const;
		const std::string& GetRootDirectory() const;
		const std::string& GetAssetsDirectory() const;
		const std::string& GetCacheDirectory() const;
		const std::string& GetCsprojDirectory() const;

		void LoadSettings();
		ProjectSettings& getSettings();
		void saveSettings();
	};
}