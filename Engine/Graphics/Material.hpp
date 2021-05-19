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
		uint32 offset;
		uint32 size;
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

		void AddParameter(const std::string& name);

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

		GraphicsApiDependent _descriptors;
		Buffer _paramsBuffer;

		bool _paramsDirty;
		bool _texturesDirty;

		MaterialTexture* GetTextureByName(const std::string& texture_name);
		MaterialParameter* GetParameterByName(const std::string& param_name);
		void CopyParamsToBuffer();
	public:

		Material() :
			_template(nullptr),
			_paramsDirty(false),
			_texturesDirty(false),
			_paramsBuffer(nullptr),
			_descriptors(nullptr)
		{}

		~Material() {
			if (_paramsBuffer)
				delete[] _paramsBuffer;
			if (_descriptors)
				delete _descriptors;
		}

		void SetTexture(const std::string& texture_name, ResourceReference* texture);
		void SetParameter(const std::string& parameter_name, MultitypeValue value);

		void SetTemplate(MaterialTemplate* mat_template);
		MaterialTemplate* GetTemplate() {
			return _template;
		}
	};
}