#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <Resources/Resource.hpp>

namespace VSGEditor {
	class ResourcePickerWindow : public EditorWindow {
	private:
		VSGE::ResourceReference* reference;
	public:

		ResourcePickerWindow();

		void SetResourceReference(VSGE::ResourceReference* reference) {
			this->reference = reference;
		}

		void OnDrawWindow();
	};
}