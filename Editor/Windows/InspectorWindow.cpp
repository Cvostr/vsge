#include "InspectorWindow.hpp"
#include <imgui_stdlib.h>

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

#include "../InspectorInterfaces/ResourcePicker.hpp"
#include "../InspectorInterfaces/VariantInput.hpp"

#include <Graphics/Material.hpp>

#include "../InspectorInterfaces/EntityComponents/EntityComponents.hpp"

using namespace VSGEditor;
using namespace VSGE;

InspectorWindow* InspectorWindow:: _this = nullptr;

template<class T>
void VSGEditor::InspectorWindow::DrawComponent() {
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
	}
}

void VSGEditor::InspectorWindow::OnDrawWindow() {
	if (Draw("Inspector")) {

		if (mShowingEntity)
			DrawEntityContents();
		if (mShowingMaterial)
			DrawMaterialContents();

		ImGui::End();
	}
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

	ViewMask current_view_mask = mShowingEntity->GetViewMask();
	for (uint64 vm_i = 0; vm_i < 32; vm_i ++) {
		bool vm = (current_view_mask >> vm_i) & 0x1;

		if (!vm) 
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(120, 120, 120, 0));

		std::string btn_id = "##vmb" + std::to_string(vm_i);
		if (ImGui::Button(btn_id.c_str(), ImVec2(10, 10))) {
			if (vm) {
				ViewMask submask = 0;
				for (uint64 sb = 0; sb < 32; sb++) {
					if (sb != vm_i)
						submask |= ((uint64)1 << sb);
				}
				current_view_mask &= submask;
			}
			else {
				current_view_mask |= ((uint64)1 << vm_i);
			}
			mShowingEntity->SetViewMask(current_view_mask);
		}
		if(!vm)
			ImGui::PopStyleColor();
		if ((vm_i + 1) % 16 > 0)
			ImGui::SameLine();
	}

    ImGui::Separator();
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

	//for (uint32 comp_i = 0; comp_i < mShowingEntity->GetComponentsCount(); comp_i) {
	//	auto component = mShowingEntity->GetComponents()[comp_i];
		//DrawComponent(component, "Mesh Renderer");
	//}

	DrawComponent<VSGE::MeshComponent>();
	DrawComponent<VSGE::MaterialComponent>();
	DrawComponent<VSGE::LightsourceComponent>();
	DrawComponent<VSGE::AnimatorComponent>();
	DrawComponent<VSGE::ParticleEmitterComponent>();
	DrawComponent<VSGE::AudioSourceComponent>();
	DrawComponent<VSGE::ColliderComponent>();
	DrawComponent<VSGE::RigidBodyComponent>();
	DrawComponent<VSGE::Camera>();

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

		ImGui::EndPopup();
	}
}

void VSGEditor::InspectorWindow::DrawMaterialContents() {
	MaterialTemplateCache* mtemplates = MaterialTemplateCache::Get();

	MaterialTemplate* _template = mShowingMaterial->GetTemplate();
	std::string current_template_name = "";
	if(_template)
		current_template_name = _template->GetName();

	if (ImGui::BeginCombo("##Template", current_template_name.c_str())) {
		for (auto mtemplate : mtemplates->GetTemplates()) {
			bool is_selected = (current_template_name == mtemplate->GetName());
			if (ImGui::Selectable(mtemplate->GetName().c_str(), is_selected)) {
				current_template_name = mtemplate->GetName();
				mShowingMaterial->SetTemplate(current_template_name);
				_template = mShowingMaterial->GetTemplate();
			}
		}

		ImGui::EndCombo();
	}

	if (_template == nullptr)
		return;

	for (MaterialTexture& texture : mShowingMaterial->GetTextures()) {
		DrawResourcePicker(texture._name.c_str(), texture._resource);
		mShowingMaterial->SetTexture(texture._name, texture._resource);
	}

	for (MaterialParameter& param : mShowingMaterial->GetParams()) {
		if (param.name[0] != '@') {
			DrawVariantInput(param.name, param.value);
			mShowingMaterial->SetParameter(param.name, param.value);
		}
	}
}