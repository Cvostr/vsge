#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <Resources/Resource.hpp>
#include <Scene/EntityComponents/EntityScriptComponent.hpp>
#include <Resources/ResourceReference.hpp>

namespace VSGEditor {
	class ResourcePickerWindow : public EditorWindow {
	private:
		VSGE::ResourceReference* reference;
		VSGE::EntityScriptComponent* script_reference;
		bool _allow_empty_resource;

		void DrawResourcePicker();
		void DrawScriptPicker();
	public:

		ResourcePickerWindow();

		void SetResourceReference(VSGE::ResourceReference* reference);

		void SetScriptReference(VSGE::EntityScriptComponent* script);

		void SetAllowEmptyResource(bool empty_resource);

		void SetResourceToReference(VSGE::Resource* resource);

		void SetResourceToReference(const std::string& resource_name);

		void OnDrawWindow();
	};
}