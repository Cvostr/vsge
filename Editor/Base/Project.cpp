#include "Project.hpp"
#include <Core/FileLoader.hpp>
#include <fstream>

using namespace VSGEditor;

Project::Project(	const std::string& root,
					const std::string& assetsDir,
					const std::string& cacheDir,
					const std::string& cfgFileName,
					const std::string& csprojDir)
{
	m_rootDirectory = root;
	for (uint32 i = 0; i < m_rootDirectory.size(); i++) {
		if (m_rootDirectory[i] == '\\')
			m_rootDirectory[i] = '/';
	}
	m_assetsDirectory = root + "/" + assetsDir;
	project_cache_dir = root + "/" + cacheDir;
	m_configFilePath = root + "/" + cfgFileName;
	m_csprojDirectory = root + "/" + csprojDir;
}

void Project::LoadSettings()
{
	char* data;
	uint32 size = 0;
	bool confResult = VSGE::LoadFile(m_configFilePath, &data, &size);
	if (!confResult) {
		throw std::runtime_error("Unable to load project configuration from file " + m_configFilePath);
	}

	m_settings = ProjectSettings::loadFromJSON(std::string(data));
	delete[] data;
}

ProjectSettings& Project::getSettings()
{
	return m_settings;
}

void Project::saveSettings()
{
	std::string json;
	m_settings.saveToJSON(json);

	std::ofstream fout_app(m_configFilePath, std::ios::binary);
	fout_app.write(json.c_str(), json.size());
	fout_app.close();
}

const std::string& Project::GetName() const
{
	return m_settings.getProjectName();
}
const std::string& Project::GetRootDirectory() const {
	return m_rootDirectory;
}
const std::string& Project::GetAssetsDirectory() const {
	return m_assetsDirectory;
}
const std::string& Project::GetCacheDirectory() const {
	return project_cache_dir;
}
const std::string& Project::GetCsprojDirectory() const
{
	return m_csprojDirectory;
}