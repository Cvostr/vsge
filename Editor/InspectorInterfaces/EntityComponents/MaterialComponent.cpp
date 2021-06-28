#include "EntityComponents.hpp"
#include <Resources/Resource.hpp>
#include "../ResourcePicker.hpp"
#include "../MaterialEditor.hpp"

using namespace VSGE;

void VSGEditor::DrawMaterialComponent(VSGE::MaterialComponent* mat) {
	DrawResourcePicker(std::string("Material"), mat->GetResourceReference());

	ImGui::Separator();
	if(mat->GetMaterialResource() != nullptr)
		DrawMaterialEditor(mat->GetMaterialResource()->GetMaterial());
}