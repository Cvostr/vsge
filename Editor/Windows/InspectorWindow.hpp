#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <Scene/Entity.hpp>
#include <Resources/ResourceTypes/MaterialResource.hpp>
#include <Scene/EntityComponents/EntityScriptComponent.hpp>

namespace VSGEditor {
	class InspectorWindow : public EditorWindow {
	private:
		static InspectorWindow* _this;
		
		VSGE::Entity* mShowingEntity;
		VSGE::Resource* mShowingResource;
	public:
		void SetShowingEntity(VSGE::Entity* entity) {
			mShowingEntity = entity;
			mShowingResource = nullptr;
		}

		void SetShowingResource(VSGE::Resource* resource) {
			mShowingEntity = nullptr;
			mShowingResource = resource;
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

		InspectorWindow() :
			mShowingEntity(nullptr),
			mShowingResource(nullptr)
		{
			_this = this;
		}
	};
}