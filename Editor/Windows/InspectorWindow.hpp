#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <Scene/Entity.hpp>
#include <Graphics/Material.hpp>
#include <Scene/EntityComponents/EntityScriptComponent.hpp>

namespace VSGEditor {
	class InspectorWindow : public EditorWindow {
	private:
		static InspectorWindow* _this;
		
		VSGE::Entity* mShowingEntity;
		VSGE::Material* mShowingMaterial;
	public:
		void SetShowingEntity(VSGE::Entity* entity) {
			mShowingEntity = entity;
			mShowingMaterial = nullptr;
		}

		void SetShowingMaterial(VSGE::Material* mat) {
			mShowingEntity = nullptr;
			mShowingMaterial = mat;
		}

		void OnDrawWindow();
		void DrawEntityContents();

		static InspectorWindow* Get()
		{
			return _this;
		}

		template<class T>
		void AddComponentButton() {
			if (!mShowingEntity->HasComponent<T>()) {
				if (ImGui::Selectable(T::GetTypeStringStatic().c_str()))
				{
					mShowingEntity->AddComponent<T>();
				}
			}
		}

		void AddScriptButton();

		template<class T>
		void DrawComponent();

		void DrawScript(VSGE::EntityScriptComponent* script);

		InspectorWindow() :
			mShowingEntity(nullptr),
			mShowingMaterial(nullptr)
		{
			_this = this;
		}
	};
}