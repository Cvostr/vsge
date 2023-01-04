#pragma once

#include <vector>
#include "MaterialTemplate.hpp"

namespace VSGE {
    typedef std::vector<MaterialTemplate*> tMaterialTemplateList;

	class MaterialTemplateCache {
	private:

		static MaterialTemplateCache* _this;

		tMaterialTemplateList _templates;
	public:

		MaterialTemplateCache();

		tMaterialTemplateList& GetTemplates();

		static MaterialTemplateCache* Get() {
			return _this;
		}

		void AddTemplate(MaterialTemplate* _template);

		MaterialTemplate* GetTemplate(const std::string& name);
	};
}