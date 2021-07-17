#include "ScriptPicker.hpp"
#include "../Windows/ResourcePickWindow.hpp"
#include "../EditorLayers/ImGuiLayer.hpp"

using namespace VSGEditor;
//using namespace VSGE;

void VSGEditor::DrawScriptPicker(std::string label, std::string& script_out, bool& changed) {
	ImGui::Text(label.c_str());

	ImGui::SameLine();

	ImGui::Text(script_out.c_str());

	ImGui::SameLine();

	std::string btn_text = "Select " + label;

	if (ImGui::Button(btn_text.c_str())) {
		ResourcePickerWindow* rpw = ImGuiLayer::Get()->GetWindow<ResourcePickerWindow>();
		rpw->SetScriptReference(&script_out);
		rpw->Show();
	}
}