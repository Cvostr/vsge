#include "InspectorWindow.hpp"
#include <imgui_stdlib.h>

#include <Scene/EntityComponents/MeshComponent.hpp>
#include <Scene/EntityComponents/MaterialComponent.hpp>
#include <Scene/EntityComponents/LightComponent.hpp>

using namespace VSGEditor;
using namespace VSGE;

InspectorWindow* InspectorWindow:: _this = nullptr;

void VSGEditor::InspectorWindow::OnDrawWindow() {
    ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoCollapse);
    UpdateWindowData();

    if (mShowingEntity)
        DrawEntityContents();

    ImGui::End();
}

void VSGEditor::InspectorWindow::DrawEntityContents() {
    bool IsActive = mShowingEntity->IsActive();
    ImGui::Checkbox("Active", &IsActive);
    mShowingEntity->SetActive(IsActive);

    ImGui::InputText("Entity Name", (std::string*)&mShowingEntity->GetName());

    ImGui::Separator();
    ImGui::InputFloat3("Translation", (float*)&mShowingEntity->GetPosition().x);
    ImGui::InputFloat3("Scale", (float*)&mShowingEntity->GetScale().x);
    ImGui::InputFloat3("Rotation", (float*)&mShowingEntity->GetRotation().x);
    ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0.45f, 0.5f, 1));
	if (ImGui::Button("Add Component", ImVec2(ImGui::GetWindowWidth(), 0)))
		ImGui::OpenPopup("add_com");
	ImGui::PopStyleColor();

	if (ImGui::BeginPopup("add_com"))
	{
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 0.7f), "--------------------- Components ---------------------");
		// # Mesh  ---------------------------------------------------------------------------
		//ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 0.7f), "# Mesh");
		if (ImGui::Selectable("Mesh Renderer"))
		{
			mShowingEntity->AddComponent<MeshComponent>();
		}
		if (ImGui::Selectable("Material"))
		{
			mShowingEntity->AddComponent<MaterialComponent>();
		}
		if (ImGui::Selectable("Light"))
		{
			mShowingEntity->AddComponent<LightComponent>();
		}
		ImGui::EndPopup();
	}
}