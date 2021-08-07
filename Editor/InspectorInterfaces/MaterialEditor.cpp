#include "MaterialEditor.hpp"
#include "VariantInput.hpp"
#include "ResourcePicker.hpp"
#include <imgui.h>

using namespace VSGE;

void VSGEditor::DrawMaterialEditor(VSGE::Material* material) {
	MaterialTemplateCache* mtemplates = MaterialTemplateCache::Get();

	MaterialTemplate* _template = material->GetTemplate();
	std::string current_template_name = "";
	if (_template)
		current_template_name = _template->GetName();

	if (ImGui::BeginCombo("##Template", current_template_name.c_str())) {
		for (auto mtemplate : mtemplates->GetTemplates()) {
			bool is_selected = (current_template_name == mtemplate->GetName());
			if (ImGui::Selectable(mtemplate->GetName().c_str(), is_selected)) {
				current_template_name = mtemplate->GetName();
				material->SetTemplate(current_template_name);
				_template = material->GetTemplate();
			}
		}

		ImGui::EndCombo();
	}

	if (_template == nullptr)
		return;

	for (MaterialTexture& texture : material->GetTextures()) {
		DrawResourcePicker(texture._name.c_str(), texture._resource, true);
		material->SetTexture(texture._name, texture._resource);
	}

	std::string captions[6] = { "Right", "Left", "Top", "Bottom", "Back", "Front" };

	for (MaterialCubeTexture& texture : material->GetCubeTextures()) {
		std::vector<ResourceReference> sides;
		for (uint32 i = 0; i < 6; i++) {
			ResourceReference* reference = &texture._cube_sides[i];
			DrawResourcePicker(captions[i].c_str(), *reference);
			sides.push_back(*reference);
		}
		
		material->SetCubeTexture(texture._name, sides.data());
	}

	for (MaterialParameter& param : material->GetParams()) {
		if (param.name[0] != '@') {
			DrawVariantInput(param.name, param.value);
			material->SetParameter(param.name, param.value);
		}
	}
}