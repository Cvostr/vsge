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
		std::string _name;
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

		const std::string& GetName() {
			return _name;
		}

		void SetName(const std::string& name) {
			_name = name;
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

	class MaterialTemplateCache {
	private:

		static MaterialTemplateCache* _this;

		std::vector<MaterialTemplate*> _templates;
	public:

		MaterialTemplateCache() {
			_this = this;
		}

		static MaterialTemplateCache* Get() {
			return _this;
		}

		void AddTemplate(MaterialTemplate* _template) {
			_templates.push_back(_template);
		}

		MaterialTemplate* GetTemplate(const std::string& name);
	};

	class Material {
	private:
		tMaterialTexturesList _materialTextures;
		tMaterialParamsList _materialParams;
		MaterialTemplate* _template;

		GraphicsApiDependent _descriptors;

		MaterialTexture* GetTextureByName(const std::string& texture_name);
		MaterialParameter* GetParameterByName(const std::string& param_name);
		uint32 CopyParamsToBuffer(char** out);
	public:

		bool _paramsDirty;
		bool _texturesDirty;

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

		tMaterialTexturesList& GetTextures() {
			return _materialTextures;
		}

		/// <summary>
		/// Set texture resource to slot with specified name
		/// </summary>
		/// <param name="texture_name">- name of texture slot</param>
		/// <param name="texture">- reference to texture resource</param>
		void SetTexture(const std::string& texture_name, ResourceReference& texture);
		/// <summary>
		/// Set param to slot with specified name
		/// </summary>
		/// <param name="parameter_name"></param>
		/// <param name="value"></param>
		void SetParameter(const std::string& parameter_name, MultitypeValue value);
		
		void SetDescriptors(GraphicsApiDependent descriptors) {
			_descriptors = descriptors;
		}

		GraphicsApiDependent GetDescriptors() {
			return _descriptors;
		}

		/// <summary>
		/// Recreate material with new material template
		/// </summary>
		/// <param name="mat_template">- pointer to template</param>
		void SetTemplate(MaterialTemplate* mat_template);
		/// <summary>
		/// Recreate material with new material template
		/// with specified name in template cache
		/// </summary>
		/// <param name="template_name">- name of template in cache</param>
		void SetTemplate(const std::string& template_name);

		MaterialTemplate* GetTemplate() {
			return _template;
		}
	};
}