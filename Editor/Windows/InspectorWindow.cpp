#include "InspectorWindow.hpp"
#include <imgui_stdlib.h>

#include "Scene/Scene.hpp"

#include <Scene/EntityComponents/MeshComponent.hpp>
#include <Scene/EntityComponents/MaterialComponent.hpp>
#include <Scene/EntityComponents/LightComponent.hpp>
#include <Scene/EntityComponents/AnimatorComponent.hpp>

#include "../InspectorInterfaces/EntityComponents/Lighsource.hpp"

using namespace VSGEditor;
using namespace VSGE;

InspectorWindow* InspectorWindow:: _this = nullptr;

template<class T>
void VSGEditor::InspectorWindow::DrawComponent(std::string Label) {
	if (mShowingEntity->HasComponent<T>()) {
		bool is_down = ImGui::CollapsingHeader(Label.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

		if (!is_down)
			return;

		if (ImGui::IsItemClicked(1))
		{
			ImGui::OpenPopup(Label.c_str());
		}

		T* component = mShowingEntity->GetComponent<T>();

		bool removeComponent = false;
		if (ImGui::BeginPopup(Label.c_str()))
		{
			if (ImGui::MenuItem("Remove component"))
				removeComponent = true;

			ImGui::EndPopup();
		}

		if (removeComponent) {
			mShowingEntity->RemoveComponent(component);
			return;
		}

		if (typeid(T) == typeid(LightsourceComponent)) {
			DrawLightsourceComponent((LightsourceComponent*)component);
		}
	}
}

void VSGEditor::InspectorWindow::OnDrawWindow() {
    ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoCollapse);
    UpdateWindowData();

    if (mShowingEntity)
        DrawEntityContents();

    ImGui::End();
}

void VSGEditor::InspectorWindow::DrawEntityContents() {
    bool IsActive = mShowingEntity->IsActive();
	bool IsStatic = mShowingEntity->IsStatic();
    ImGui::Checkbox("Active", &IsActive);
	ImGui::SameLine();
	ImGui::Checkbox("Static", &IsStatic);
    mShowingEntity->SetActive(IsActive);
	mShowingEntity->SetStatic(IsStatic);

	std::string new_name = mShowingEntity->GetName();
    ImGui::InputText("Entity Name", &new_name);
	mShowingEntity->SetName(new_name);

    ImGui::Separator();
    ImGui::InputFloat3("Translation", (float*)&mShowingEntity->GetPosition().x);
    ImGui::InputFloat3("Scale", (float*)&mShowingEntity->GetScale().x);
    ImGui::InputFloat3("Rotation", (float*)&mShowingEntity->GetRotation().x);
    ImGui::Separator();

	//for (uint32 comp_i = 0; comp_i < mShowingEntity->GetComponentsCount(); comp_i) {
	//	auto component = mShowingEntity->GetComponents()[comp_i];
		//DrawComponent(component, "Mesh Renderer");
	//}

	DrawComponent<VSGE::MeshComponent>("Mesh Renderer");
	DrawComponent<VSGE::MaterialComponent>("Material");
	DrawComponent<VSGE::LightsourceComponent>("Light");
	DrawComponent<VSGE::AnimatorComponent>("Animator");

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0.45f, 0.5f, 1));
	if (ImGui::Button("Add Component", ImVec2(ImGui::GetWindowWidth(), 0)))
		ImGui::OpenPopup("add_com");
	ImGui::PopStyleColor();

	if (ImGui::BeginPopup("add_com"))
	{
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 0.7f), "--------------------- Components ---------------------");
		
		AddComponentButton<VSGE::MeshComponent>("Mesh Renderer");
		AddComponentButton<VSGE::MaterialComponent>("Material");
		AddComponentButton<VSGE::LightsourceComponent>("Light");
		AddComponentButton<VSGE::AnimatorComponent>("Animator");

		ImGui::EndPopup();
	}
}