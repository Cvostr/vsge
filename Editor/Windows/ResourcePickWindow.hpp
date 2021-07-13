#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <Resources/Resource.hpp>

namespace VSGEditor {
	class ResourcePickerWindow : public EditorWindow {
	private:
		VSGE::ResourceReference* reference;
		std::string* script_name;
	public:

		ResourcePickerWindow();

		void SetResourceReference(VSGE::ResourceReference* reference);

		void SetScriptReference(std::string* script);

		void SetResourceToReference(VSGE::Resource* resource);

		void OnDrawWindow();
	};
}