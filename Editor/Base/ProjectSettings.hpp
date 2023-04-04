#pragma once

#include <string>
#include <Resources/ResourceReference.hpp>

namespace VSGEditor
{
	class ProjectSettings
	{
	private:
		std::string m_projectName;
		std::string m_applicationName;
		std::string m_versionString;
		int m_version;
		VSGE::ResourceReference m_startupScene;
	public:
		void setProjectName(const std::string& projectName);
		void setApplicationName(const std::string& applicationName);
		void setVersionString(const std::string& versionString);
		void setVersion(int version);
		void setStartupSceneId(const Guid& resourceId);

		const std::string& getProjectName() const;
		const std::string& getApplicationName() const;
		const std::string& getVersionString() const;
		int getVersion() const;
		VSGE::ResourceReference& getStartupSceneResource();

		void saveToJSON(std::string& jsonResult);

		static const ProjectSettings loadFromJSON(const std::string& jsonDocumentString);
	};
}