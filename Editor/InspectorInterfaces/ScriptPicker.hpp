#pragma once

#include <imgui.h>
#include <string>
#include <Scripting/Angel/AngelScriptInstance.hpp>
#include <Scene/EntityComponents/EntityScriptComponent.hpp>

namespace VSGEditor {

	void DrawScriptPicker(std::string label, VSGE::EntityScriptComponent* script);
}