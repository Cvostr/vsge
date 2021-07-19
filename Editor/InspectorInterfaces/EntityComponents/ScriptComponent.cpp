#include "EntityComponents.hpp"
#include "../ScriptPicker.hpp"

using namespace VSGEditor;

void VSGEditor::DrawScriptComponent(VSGE::EntityScriptComponent* esc) {
	
	DrawScriptPicker("Script", esc);
}