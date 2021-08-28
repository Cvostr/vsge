#pragma once

#include <Resources/Resource.hpp>
#include <Resources/ResourceReference.hpp>
#include <imgui.h>

namespace VSGEditor {

	void DrawResourcePicker(std::string label, VSGE::ResourceReference& reference, bool empty_res = false, uint32 index = 0);
}