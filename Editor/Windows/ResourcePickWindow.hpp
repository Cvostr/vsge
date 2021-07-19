#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <Resources/Resource.hpp>
#include <Scene/EntityComponents/EntityScriptComponent.hpp>

namespace VSGEditor {
	class ResourcePickerWindow : public EditorWindow {
	private:
		VSGE::ResourceReference* reference;
		VSGE::EntityScriptComponent* script;
	public:

		ResourcePickerWindow();

		void SetResourceReference(VSGE::ResourceReference* reference);

		void SetScriptReference(VSGE::EntityScriptComponent* script);

		void SetResourceToReference(VSGE::Resource* resource);

		void OnDrawWindow();
	};
}