#pragma once

#include <imgui.h>
#include <string>
#include <Scripting/Angel/AngelScriptInstance.hpp>

namespace VSGEditor {

	void DrawScriptPicker(std::string label, std::string& script_out, bool& changed);
}