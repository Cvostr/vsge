#include <Engine/Application.hpp>
#include "EditorSettingsLayer.hpp"
#include <fstream>
#include <mpi/Parse/Json/JsonReader.hpp>
#include <mpi/Parse/Json/JsonWriter.hpp>

using namespace VSGEditor;
using namespace Mpi;

#define SETTINGS_FILE "settings.json"

EditorSettingsLayer* EditorSettingsLayer::_this = nullptr;

void EditorSettingsLayer::parseJSON(const std::string& settingsJson)
{
	JsonReader reader(settingsJson);
	JsonNode node = reader.parse();

	_windowWidth = (uint32)node["windowWidth"].getValue<int>();
	_windowHeight = (uint32)node["windowHeight"].getValue<int>();

	_windowPosX = (uint32)node["windowPosX"].getValue<int>();
	_windowPosY = (uint32)node["windowPosY"].getValue<int>();

	icons_size = node["iconSize"].getValue<int>();
}

void EditorSettingsLayer::serializeToJSON(std::string& serialized)
{
	Mpi::JsonNode cfgRootNode;
	cfgRootNode
		.add("windowWidth", Mpi::JsonValue((int)_windowWidth))
		.add("windowHeight", Mpi::JsonValue((int)_windowHeight))
		.add("windowPosX", Mpi::JsonValue((int)_windowPosX))
		.add("windowPosY", Mpi::JsonValue((int)_windowPosY))
		.add("iconSize", Mpi::JsonValue(icons_size));
	Mpi::JsonWriter writer(cfgRootNode);
	writer.write(serialized);
}

void EditorSettingsLayer::OnAttach() 
{
	Mpi::File file(SETTINGS_FILE);
	if (file.isFile()) 
	{
		std::ifstream stream = file.getIfstream(std::ios::binary);
		char* data = new char[file.getFileSize()];
		stream.read(data, file.getFileSize());
		stream.close();
		parseJSON(std::string(data));
		delete[] data;
	}
}

void EditorSettingsLayer::OnUpdate() {

}
void EditorSettingsLayer::OnDetach() {
	auto app = VSGE::Application::Get();
	auto win = app->GetWindow();
	//Save

	_windowWidth = win.GetWindowWidth();
	_windowHeight = win.GetWindowHeight();
	_windowPosX = win.GetWindowPositionX();
	_windowPosY = win.GetWindowPositionX();

	std::string json;
	serializeToJSON(json);

	Mpi::File file(SETTINGS_FILE);
	std::ofstream stream = file.getOfstream(std::ios::binary);
	stream.write(json.c_str(), json.size());
	stream.close();

}