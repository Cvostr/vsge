#include <Engine/Application.hpp>
#include "EditorSettingsLayer.hpp"
#include <yaml-cpp/yaml.h>
#include <fstream>

using namespace VSGEditor;

EditorSettingsLayer* EditorSettingsLayer::_this = nullptr;

void EditorSettingsLayer::OnAttach() {
	YAML::Node data;
	try {
		data = YAML::LoadFile("editor_settings.ini");
	}
	catch (YAML::BadFile& exc) {
		return;
	}

	icons_size = data["icons_size"].as<uint32>();
	_mono_path = data["mono_path"].as<std::string>();

	_windowWidth = data["win_width"].as<uint32>();
	_windowHeight = data["win_height"].as<uint32>();

	_windowPosX = data["win_posX"].as<uint32>();
	_windowPosY = data["win_posY"].as<uint32>();
}
void EditorSettingsLayer::OnUpdate() {

}
void EditorSettingsLayer::OnDetach() {
	auto app = VSGE::Application::Get();
	auto win = app->GetWindow();
	//Save

	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "icons_size" << YAML::Value << icons_size;
	out << YAML::Key << "mono_path" << YAML::Value << _mono_path;
	out << YAML::Key << "win_posX" << YAML::Value << win.GetWindowPositionX();
	out << YAML::Key << "win_posY" << YAML::Value << win.GetWindowPositionY();
	out << YAML::Key << "win_width" << YAML::Value << win.GetWindowWidth();
	out << YAML::Key << "win_height" << YAML::Value << win.GetWindowHeight();

	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout("editor_settings.ini");
	fout << out.c_str();
}