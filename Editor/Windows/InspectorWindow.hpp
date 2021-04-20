#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <Scene/Entity.hpp>

namespace VSGEditor {
	class InspectorWindow : public EditorWindow {
	private:
		static InspectorWindow* _this;
		
		VSGE::Entity* mShowingEntity;
	public:
		void SetShowingEntity(VSGE::Entity* entity) {
			mShowingEntity = entity;
		}

		void OnDrawWindow();
		void DrawEntityContents();

		static InspectorWindow* Get()
		{
			return _this;
		}

		template<class T>
		void AddComponentButton(std::string Label) {
			if (!mShowingEntity->HasComponent<T>()) {
				if (ImGui::Selectable(Label.c_str()))
				{
					mShowingEntity->AddComponent<T>();
				}
			}
		}

		InspectorWindow() :
			mShowingEntity(nullptr) 
		{
			_this = this;
		}
	};
}