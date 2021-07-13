#include "EntityComponents.hpp"
#include "../ScriptPicker.hpp"

using namespace VSGEditor;

void VSGEditor::DrawScriptComponent(VSGE::EntityScriptComponent* esc) {
	std::string class_name = esc->GetScriptName();
	std::string temp = class_name;
	bool changed = false;
	DrawScriptPicker("Script", temp, changed);

	if (temp != class_name) {
		esc->SetScriptName(temp);
	}
}