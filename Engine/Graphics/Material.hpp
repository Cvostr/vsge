#pragma once

#include <Resources/ResourceTypes/TextureResource.hpp>
#include <Core/VarTypes/MultitypeValue.hpp>
#include "Shader.hpp"
#include "VertexLayout.hpp"
#include <vector>

namespace VSGE {

	class MaterialParameter {
	public:
		std::string name;
		MultitypeValue value;
	};

	class MaterialTexture {
		std::string name;
		ResourceReference<TextureResource> resource;
	};

	class MaterialTemplate {
	private:
		std::vector<MaterialTexture> _materialTextures;
		std::vector<MaterialParameter> _materialParams;

		Shader* _shader;
		VertexLayout vl;
	public:
		
		Shader* GetShader() {
			return _shader;
		}

		VertexLayout& GetLayout() {
			return vl;
		}

		MaterialTemplate() {

		}
	};

	class Material {
	private:
		std::vector<MaterialTexture> _materialTextures;
		std::vector<MaterialParameter> _materialParams;
	public:
		void SetTemplate(MaterialTemplate* mat_template);
	};
}