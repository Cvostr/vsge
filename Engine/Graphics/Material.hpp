#pragma once

#include <Resources/ResourceTypes/TextureResource.hpp>
#include <Core/VarTypes/MultitypeValue.hpp>
#include "Shader.hpp"
#include <Core/VarTypes/Guid.hpp>
#include "VertexLayout.hpp"
#include <vector>
#include "GraphicsPipeline.hpp"
#include "GpuBuffer.hpp"

namespace VSGE {

	class MaterialParameter {
	public:
		std::string name;
		MultitypeValue value;

		MaterialParameter() {

		}
	};

	class MaterialTexture {
	public:
		std::string _name;
		ResourceReference _resource;
		uint32 _binding;

		MaterialTexture() {
			_binding = 0;
			_resource.SetResourceType(RESOURCE_TYPE_TEXTURE);
		}
	};

	typedef std::vector<MaterialTexture> tMaterialTexturesList;
	typedef std::vector<MaterialParameter> tMaterialParamsList;

	class MaterialTemplate {
	private:
		tMaterialTexturesList _materialTextures;
		tMaterialParamsList _materialParams;

		Shader* _shader;
		VertexLayout _vertexLayout;
		GraphicsPipeline* _pipeline;

		void SetupDefaultVertexLayout();
	public:
		
		MaterialTemplate() :
			_shader(nullptr),
			_pipeline(nullptr)
		{
			SetupDefaultVertexLayout();
		}

		void SetPipeline(GraphicsPipeline* pipeline) {
			_pipeline = pipeline;
		}

		GraphicsPipeline* GetPipeline() {
			return _pipeline;
		}

		tMaterialTexturesList& GetTextures() {
			return _materialTextures;
		}

		tMaterialParamsList& GetParams() {
			return _materialParams;
		}

		void AddParameter(const std::string& name, MultitypeValue baseValue);
		
		void AddTexture(const std::string& name, uint32 binding);

		Shader* GetShader() {
			return _shader;
		}

		void SetShader(Shader* shader) {
			_shader = shader;
		}

		void SetShader(const std::string& shader_name);

		VertexLayout& GetLayout() {
			return _vertexLayout;
		}
	};

	class Material {
	private:
		tMaterialTexturesList _materialTextures;
		tMaterialParamsList _materialParams;
		MaterialTemplate* _template;

		GraphicsApiDependent _descriptors;

		bool _paramsDirty;
		bool _texturesDirty;

		MaterialTexture* GetTextureByName(const std::string& texture_name);
		MaterialParameter* GetParameterByName(const std::string& param_name);
		uint32 CopyParamsToBuffer(char** out);
	public:

		Material() :
			_template(nullptr),
			_paramsDirty(false),
			_texturesDirty(false),
			_descriptors(nullptr)
		{}

		~Material() {
			if (_descriptors)
				delete _descriptors;
		}

		void SetTexture(const std::string& texture_name, ResourceReference* texture);
		void SetParameter(const std::string& parameter_name, MultitypeValue value);

		void SetDescriptors(GraphicsApiDependent descriptors) {
			_descriptors = descriptors;
		}

		void SetTemplate(MaterialTemplate* mat_template);
		MaterialTemplate* GetTemplate() {
			return _template;
		}
	};
}