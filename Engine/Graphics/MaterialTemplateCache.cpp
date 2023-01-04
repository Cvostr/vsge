#include "MaterialTemplateCache.hpp"

using namespace VSGE;

MaterialTemplateCache* MaterialTemplateCache::_this = nullptr;

MaterialTemplateCache template_cache;

MaterialTemplateCache::MaterialTemplateCache() {
	_this = this;
}

tMaterialTemplateList& MaterialTemplateCache::GetTemplates() {
    return _templates;
}

void MaterialTemplateCache::AddTemplate(MaterialTemplate* _template) {
	_templates.push_back(_template);
}

MaterialTemplate* MaterialTemplateCache::GetTemplate(const std::string& name) {
	for (auto _template : _templates) {
		if (_template->GetName() == name) {
			return _template;
		}
	}
	return nullptr;
}