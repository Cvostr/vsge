#include "EntityComponents.hpp"
#include <Resources/Resource.hpp>
#include "../ResourcePicker.hpp"

using namespace VSGE;

void VSGEditor::DrawMaterialComponent(VSGE::MaterialComponent* mat) {
	DrawResourcePicker(std::string("Material"), mat->GetResourceReference());
}