#include "ProjectSettings.hpp"
#include <mpi/Core/Guid.hpp>
#include <mpi/Parse/Json/JsonReader.hpp>
#include <mpi/Parse/Json/JsonWriter.hpp>

using namespace VSGEditor;
using namespace Mpi;

const ProjectSettings ProjectSettings::loadFromJSON(const std::string& jsonDocumentString)
{
	ProjectSettings result;
	JsonReader reader(jsonDocumentString);
	JsonNode node = reader.parse();

	result.m_projectName = node["projectName"].getValue<std::string>();
	result.m_applicationName = node["applicationName"].getValue<std::string>();
	result.m_versionString = node["versionString"].getValue<std::string>();
	result.m_version = node["version"].getValue<int>();
	result.m_startupScene.SetResource( Guid(node["startupScene"].getValue<std::string>()) );

	return result;
}

void ProjectSettings::saveToJSON(std::string& jsonResult)
{
	Mpi::JsonNode cfgRootNode;
	cfgRootNode
		.add("projectName", Mpi::JsonValue(m_projectName))
		.add("applicationName", Mpi::JsonValue(m_applicationName))
		.add("version", Mpi::JsonValue(m_version))
		.add("versionString", Mpi::JsonValue(m_versionString))
		.add("startupScene", Mpi::JsonValue(m_startupScene.GetId().toString()));
	Mpi::JsonWriter writer(cfgRootNode);
	writer.write(jsonResult);
}

void ProjectSettings::setProjectName(const std::string& projectName)
{
	m_projectName = projectName;
}
void ProjectSettings::setApplicationName(const std::string& applicationName)
{
	m_applicationName = applicationName;
}
void ProjectSettings::setVersionString(const std::string& versionString)
{
	m_versionString = versionString;
}
void ProjectSettings::setVersion(int version)
{
	m_version = version;
}

void ProjectSettings::setStartupSceneId(const Guid& resourceId)
{
	m_startupScene.SetResource(Guid(0, 0, 0, 0));
}

const std::string& ProjectSettings::getProjectName() const
{
	return m_projectName;
}
const std::string& ProjectSettings::getApplicationName() const
{
	return m_applicationName;
}
const std::string& ProjectSettings::getVersionString() const
{
	return m_versionString;
}
int ProjectSettings::getVersion() const
{
	return m_version;
}
VSGE::ResourceReference& ProjectSettings::getStartupSceneResource()
{
	return m_startupScene;
}