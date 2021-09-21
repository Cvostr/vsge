#include "InspectorWindow.hpp"
#include <imgui_stdlib.h>
#include "../Base/ImguiHelper.hpp"
#include "../InspectorInterfaces/ViewMaskEdit.hpp"
#include "../InspectorInterfaces/MaterialEditor.hpp"

#include "Scene/Scene.hpp"

#include <Scene/EntityComponents/MeshComponent.hpp>
#include <Scene/EntityComponents/MaterialComponent.hpp>
#include <Scene/EntityComponents/LightComponent.hpp>
#include <Scene/EntityComponents/AnimatorComponent.hpp>
#include <Scene/EntityComponents/ParticleEmitterComponent.hpp>
#include <Scene/EntityComponents/AudioSourceComponent.hpp>
#include <Scene/EntityComponents/ColliderComponent.hpp>
#include <Scene/EntityComponents/RigidBodyComponent.hpp>
#include <Scene/Camera.hpp>
#include <Scene/EntityComponents/TerrainComponent.hpp>

#include "../InspectorInterfaces/ResourcePicker.hpp"
#include "../InspectorInterfaces/VariantInput.hpp"

#include <Graphics/Material.hpp>

#include "../InspectorInterfaces/EntityComponents/EntityComponents.hpp"
#include "../InspectorInterfaces/ResourceTypes/InspectorResourceTypes.hpp"

using namespace VSGEditor;
using namespace VSGE;

InspectorWindow* InspectorWindow:: _this = nullptr;

template<class T>
void InspectorWindow::DrawComponent() {
	if (mShowingEntity->HasComponent<T>()) {

		T* component = mShowingEntity->GetComponent<T>();

		if (!component->IsActive()) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1));
		}
		bool is_down = ImGui::CollapsingHeader(T::GetTypeStringStatic().c_str(), ImGuiTreeNodeFlags_DefaultOpen);

		if (!component->IsActive()) {
			ImGui::PopStyleColor();
		}

		if (!is_down)
			return;

		if (ImGui::IsItemClicked(1))
		{
			ImGui::OpenPopup(T::GetTypeStringStatic().c_str());
		}

		bool removeComponent = false;
		if (ImGui::BeginPopup(T::GetTypeStringStatic().c_str()))
		{
			bool active = component->IsActive();
			ImGui::MenuItem("Active", NULL, &active);
			component->SetActive(active);

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
		if (typeid(T) == typeid(MeshComponent)) {
			DrawMeshComponent((MeshComponent*)component);
		}
		if (typeid(T) == typeid(MaterialComponent)) {
			DrawMaterialComponent((MaterialComponent*)component);
		}
		if (typeid(T) == typeid(ParticleEmitterComponent)) {
			DrawParticleEmitterComponent((ParticleEmitterComponent*)component);
		}
		if (typeid(T) == typeid(AnimatorComponent)) {
			DrawAnimatorComponent((AnimatorComponent*)component);
		}
		if (typeid(T) == typeid(AudioSourceComponent)) {
			DrawAudioSourceComponent((AudioSourceComponent*)component);
		}
		if (typeid(T) == typeid(RigidBodyComponent)) {
			DrawRigidbodyComponent((RigidBodyComponent*)component);
		}
		if (typeid(T) == typeid(ColliderComponent)) {
			DrawColliderComponent((ColliderComponent*)component);
		}
		if (typeid(T) == typeid(Camera)) {
			DrawCameraComponent((Camera*)component);
		}
		if (typeid(T) == typeid(TerrainComponent)) {
			DrawTerrainComponent((TerrainComponent*)component);
		}
	}
}

void InspectorWindow::DrawScript(VSGE::EntityScriptComponent* script) {

	if (!script->IsActive()) {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1));
	}

	std::string header = "Script";

	bool is_down = ImGui::CollapsingHeader(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

	if (!script->IsActive()) {
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
		bool active = script->IsActive();
		ImGui::MenuItem("Active", NULL, &active);
		script->SetActive(active);

		if (ImGui::MenuItem("Remove component"))
			removeComponent = true;

		ImGui::EndPopup();
	}

	if (removeComponent) {
		mShowingEntity->RemoveScript(script);
		return;
	}

	DrawScriptPicker("Class name", script);
}

void InspectorWindow::OnDrawWindow() {
	if (Draw("Inspector")) {

		if (mShowingEntity)
			DrawEntityContents();
		if (mShowingResource) {
			if(mShowingResource->GetResourceType() == RESOURCE_TYPE_MATERIAL)
				DrawMaterialEditor((MaterialResource*)mShowingResource);
			if (mShowingResource->GetResourceType() == RESOURCE_TYPE_TEXTURE)
				DrawTextureResourceUI(mShowingResource);
			if (mShowingResource->GetResourceType() == RESOURCE_TYPE_AUDIOCLIP)
				DrawAudioResourceUI(mShowingResource);
		}

		ImGui::End();
	}
}

void InspectorWindow::AddScriptButton() {
	if (ImGui::Selectable("Mono Script"))
	{
		mShowingEntity->AddScript(new EntityScriptComponent); 
	}
}

void InspectorWindow::DrawEntityContents() {
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

	Camera* cam_component = mShowingEntity->GetComponent<Camera>();
	if (!cam_component) {
		ViewMask current_view_mask = mShowingEntity->GetViewMask();
		DrawViewMaskEditor(current_view_mask);
		mShowingEntity->SetViewMask(current_view_mask);
		ImGui::Separator();
	}
    
	Vec3 translation = mShowingEntity->GetPosition();
    ImGui::InputFloat3("Translation", (float*)&translation.x);
	mShowingEntity->SetPosition(translation);
	Vec3 scale = mShowingEntity->GetScale();
    ImGui::InputFloat3("Scale", (float*)&scale.x);
	Vec3 rotation_euler = mShowingEntity->GetRotationEuler();
	mShowingEntity->SetScale(scale);
    ImGui::InputFloat3("Rotation", &rotation_euler.x);
	mShowingEntity->SetRotationEuler(rotation_euler);

    ImGui::Separator();


	DrawComponent<VSGE::MeshComponent>();
	DrawComponent<VSGE::MaterialComponent>();
	DrawComponent<VSGE::LightsourceComponent>();
	DrawComponent<VSGE::AnimatorComponent>();
	DrawComponent<VSGE::ParticleEmitterComponent>();
	DrawComponent<VSGE::AudioSourceComponent>();
	DrawComponent<VSGE::ColliderComponent>();
	DrawComponent<VSGE::RigidBodyComponent>();
	DrawComponent<VSGE::Camera>();
	DrawComponent<VSGE::AudioListenerComponent>();
	DrawComponent<VSGE::TerrainComponent>();

	for (uint32 script_i = 0; script_i < mShowingEntity->GetScriptsCount(); script_i++) {
		DrawScript(mShowingEntity->GetScripts()[script_i]);
	}


	ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0.45f, 0.5f, 1));
	if (ImGui::Button("Add Component", ImVec2(ImGui::GetWindowWidth(), 0)))
		ImGui::OpenPopup("add_com");
	ImGui::PopStyleColor();

	if (ImGui::BeginPopup("add_com"))
	{
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 0.7f), "--------------------- Components ---------------------");
		
		AddComponentButton<VSGE::MeshComponent>();
		AddComponentButton<VSGE::MaterialComponent>();
		AddComponentButton<VSGE::LightsourceComponent>();
		AddComponentButton<VSGE::AnimatorComponent>();
		AddComponentButton<VSGE::ParticleEmitterComponent>();
		AddComponentButton<VSGE::AudioSourceComponent>();
		AddComponentButton<VSGE::ColliderComponent>();
		AddComponentButton<VSGE::RigidBodyComponent>();
		AddComponentButton<VSGE::Camera>();
		AddComponentButton<VSGE::TerrainComponent>();

		AddScriptButton();

		ImGui::EndPopup();
	}
}