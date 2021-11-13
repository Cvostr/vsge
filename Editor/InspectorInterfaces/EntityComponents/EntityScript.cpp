#include "EntityComponents.hpp"
#include <InspectorInterfaces/ResourcePicker.hpp>
#include "../VariantInput.hpp"
#include <imgui.h>

void VSGEditor::DrawScriptComponent(VSGE::EntityScriptComponent* esc, uint32 index) {
	if (!esc->IsActive()) {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1));
	}

	std::string header = "Script";

	bool is_down = ImGui::CollapsingHeader(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

	if (!esc->IsActive()) {
		ImGui::PopStyleColor();
	}

	if (!is_down)
		return;

	if (ImGui::IsItemClicked(1))
	{
		ImGui::OpenPopup(header.c_str());
	}

	bool removeComponent = false;
	if (ImGui::BeginPopup(header.c_str()))
	{
		bool active = esc->IsActive();
		ImGui::MenuItem("Active", NULL, &active);
		esc->SetActive(active);

		if (ImGui::MenuItem("Remove component"))
			removeComponent = true;

		ImGui::EndPopup();
	}

	if (removeComponent) {
		esc->GetEntity()->RemoveScript(esc);
		return;
	}

	DrawScriptPicker("Class name", esc, index);

	for (auto& field : esc->GetFields()) {
		DrawVariantInput(field.GetDesc()->GetName(), field.GetValue());
	}
}