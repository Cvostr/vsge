#include "ResourcePicker.hpp"
#include "../Windows/ResourcePickWindow.hpp"
#include "../EditorLayers/ImGuiLayer.hpp"

using namespace VSGEditor;

void VSGEditor::DrawScriptPicker(const std::string& label, VSGE::EntityScriptComponent* script, uint32 index) {
	ImGui::Text(label.c_str());

	ImGui::SameLine();

	ImGui::Text(script->GetClassName().c_str());

	ImGui::SameLine();

	std::string btn_text = "Select##" + std::to_string(index);

	if (ImGui::Button(btn_text.c_str())) {
		ResourcePickerWindow* rpw = ImGuiLayer::Get()->GetWindow<ResourcePickerWindow>();
		rpw->SetScriptReference(script);
		rpw->Show();
	}
}