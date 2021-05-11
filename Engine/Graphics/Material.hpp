#pragma once

#include <Resources/ResourceTypes/TextureResource.hpp>
#include <Core/VarTypes/MultitypeValue.hpp>
#include "Shader.hpp"
#include <Core/VarTypes/Guid.hpp>
#include "VertexLayout.hpp"
#include <vector>

namespace VSGE {

	class MaterialParameter {
	public:
		std::string name;
		MultitypeValue value;
	};

	class MaterialTexture {
	public:
		std::string name;
		ResourceReference resource;
		uint32 _binding;

		MaterialTexture() {
			_binding = 0;
			resource.SetResourceType(RESOURCE_TYPE_TEXTURE);
		}
	};

	typedef std::vector<MaterialTexture> tMaterialTexturesList;
	typedef std::vector<MaterialParameter> tMaterialParamsList;

	class MaterialTemplate {
	private:
		Guid _templateGuid;

		tMaterialTexturesList _materialTextures;
		tMaterialParamsList _materialParams;

		Shader* _shader;
		VertexLayout _vertexLayout;

		void SetupDefaultVertexLayout();
	public:
		
		tMaterialTexturesList& GetTextures() {
			return _materialTextures;
		}

		tMaterialParamsList& GetParams() {
			return _materialParams;
		}

		Shader* GetShader() {
			return _shader;
		}

		const Guid& GetGuid() {
			return _templateGuid;
		}

		void SetShader(Shader* shader) {
			_shader = shader;
		}

		void SetShader(const std::string& shader_name);

		VertexLayout& GetLayout() {
			return _vertexLayout;
		}

		MaterialTemplate() : 
			_shader(nullptr)
		{
			SetupDefaultVertexLayout();
		}
	};

	class Material {
	private:
		tMaterialTexturesList _materialTextures;
		tMaterialParamsList _materialParams;
		MaterialTemplate* _template;
	public:
		void SetTemplate(MaterialTemplate* mat_template);
		MaterialTemplate* GetTemplate() {
			return _template;
		}
	};
}