#pragma once

#include <Resources/Resource.hpp>
#include <Resources/ResourceReference.hpp>
#include <Scene/EntityComponents/EntityScriptComponent.hpp>
#include <imgui.h>

namespace VSGEditor {
	void DrawResourcePicker(const std::string& label, VSGE::ResourceReference& reference, VSGE::ResourceType resourceType, bool empty_res = false, uint32 index = 0);
	void DrawScriptPicker(const std::string& label, VSGE::EntityScriptComponent* script, uint32 index);
}