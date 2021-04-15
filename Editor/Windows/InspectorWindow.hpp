#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <Scene/Entity.hpp>

namespace VSGEditor {
	class InspectorWindow : public ImGuiWindow {
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

		InspectorWindow() :
			mShowingEntity(nullptr) 
		{
			_this = this;
		}
	};
}