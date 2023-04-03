#include "EntityComponents.hpp"
#include <Resources/Resource.hpp>
#include "../ResourcePicker.hpp"
#include "../MaterialEditor.hpp"
#include <Base/ImguiHelper.hpp>

using namespace VSGE;

void VSGEditor::DrawMaterialComponent(VSGE::MaterialComponent* mat) {
	DrawResourcePicker(std::string("Material"), mat->GetResourceReference(), VSGE::RESOURCE_TYPE_MATERIAL);

	bool cast_shadows = mat->IsCastShadows();
	DrawCheckboxControl("Cast shadows", cast_shadows);
	mat->SetCastShadows(cast_shadows);

	if(mat->GetMaterialResource() != nullptr)
		if(mat->GetMaterialResource()->GetName() != "Default Material")
			DrawMaterialEditor(mat->GetMaterialResource());
}