#include "MaterialEditor.hpp"
#include "VariantInput.hpp"
#include "ResourcePicker.hpp"
#include <imgui.h>

using namespace VSGE;

void VSGEditor::DrawMaterialEditor(MaterialResource* material_resource) {
	MaterialTemplateCache* mtemplates = MaterialTemplateCache::Get();
	Material* material = material_resource->GetMaterial();

	if (material_resource->IsUnloaded()) {
		material_resource->Load();
		return;
	}

	if (material_resource->IsReady()) {
		material_resource->Use();
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

		for (MaterialTexture* texture : material->GetTextures()) {
			if (texture->_name[0] != '@') {
				DrawResourcePicker(texture->_name.c_str(), texture->_resource, true);
				material->SetTexture(texture->_name, texture->_resource);
			}
		}

		for (MaterialParameter& param : material->GetParams()) {
			if (param.name[0] != '@') {
				DrawVariantInput(param.name, param.value);
				material->SetParameter(param.name, param.value);
			}
		}
	}
}