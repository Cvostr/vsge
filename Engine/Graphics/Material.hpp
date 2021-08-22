#pragma once

#include <Resources/ResourceTypes/TextureResource.hpp>
#include <Core/VarTypes/MultitypeValue.hpp>
#include "Shader.hpp"
#include <Core/VarTypes/Guid.hpp>
#include "VertexLayout.hpp"
#include <vector>
#include "GraphicsPipeline.hpp"
#include "GpuBuffer.hpp"
#include <map>
#include <Resources/ResourceReference.hpp>

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

	struct TextureTypesIds {
		uint8 _albedo_index;
		uint8 _normal_index;

		TextureTypesIds() : 
			_albedo_index(0),
			_normal_index(1)
		{}
	};

	class MaterialTemplate {
	private:
		std::string _name;
		tMaterialTexturesList _materialTextures;
		tMaterialParamsList _materialParams;

		Shader* _shader;
		VertexLayout _vertexLayout;
		GraphicsPipeline* _pipeline;

		CullMode _cullMode;
		bool _depthTest;
		std::map<uint32, BlendAttachmentDesc> _blendDescs;
		TextureTypesIds _texture_ids;

		void SetupDefaultVertexLayout();
	public:
		
		MaterialTemplate();

		~MaterialTemplate();
		/// <summary>
		/// Get name of template
		/// </summary>
		/// <returns></returns>
		const std::string& GetName();
		/// <summary>
		/// Set name to material template
		/// </summary>
		/// <param name="name">- name of template</param>
		void SetName(const std::string& name);

		void SetPipeline(GraphicsPipeline* pipeline);

		GraphicsPipeline* GetPipeline();
		/// <summary>
		/// Get array of template textures
		/// </summary>
		/// <returns></returns>
		tMaterialTexturesList& GetTextures();
		/// <summary>
		/// Get array of template params
		/// </summary>
		/// <returns></returns>
		tMaterialParamsList& GetParams();

		TextureTypesIds& GetTexturesIds() {
			return _texture_ids;
		}
		/// <summary>
		/// Add new template parameter description
		/// </summary>
		/// <param name="name">- name of parameter field</param>
		/// <param name="baseValue"></param>
		void AddParameter(const std::string& name, MultitypeValue baseValue);
		/// <summary>
		/// Add new template texture description
		/// </summary>
		/// <param name="name">- name of texture field</param>
		/// <param name="binding">- binding of texture</param>
		void AddTexture(const std::string& name, uint32 binding);
		/// <summary>
		/// Get shader of material template pointer
		/// </summary>
		/// <returns></returns>
		Shader* GetShader();
		/// <summary>
		/// Set shader to material template
		/// </summary>
		/// <param name="shader"></param>
		void SetShader(Shader* shader);
		/// <summary>
		/// Set shader to material template
		/// </summary>
		/// <param name="shader"></param>
		void SetShader(const std::string& shader_name);
		/// <summary>
		/// Get current face cull mode of template
		/// </summary>
		/// <returns></returns>
		CullMode GetCullMode();
		/// <summary>
		/// Set new face cull mode to template
		/// </summary>
		/// <param name="mode">- new face cull mode</param>
		void SetCullMode(CullMode mode);

		bool GetDepthTest();

		void SetDepthTest(bool depth_test);

		void SetVertexLayout(const VertexLayout& vertexLayout);

		void SetBlendingAttachmentDesc(uint32 attachment, const BlendAttachmentDesc& desc);

		const BlendAttachmentDescsMap& GetBlendingAttachmentDescs() {
			return _blendDescs;
		}

		VertexLayout& GetLayout() {
			return _vertexLayout;
		}
	};

	typedef std::vector<MaterialTemplate*> tMaterialTemplateList;

	class MaterialTemplateCache {
	private:

		static MaterialTemplateCache* _this;

		tMaterialTemplateList _templates;
	public:

		MaterialTemplateCache() {
			_this = this;
		}

		tMaterialTemplateList& GetTemplates() {
			return _templates;
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
		
	public:

		bool _paramsDirty;
		bool _texturesDirty;

		Material();

		~Material();

		/// <summary>
		/// Get vector of textures
		/// </summary>
		/// <returns></returns>
		tMaterialTexturesList& GetTextures() {
			return _materialTextures;
		}
		/// <summary>
		/// Get vector of parameters
		/// </summary>
		/// <returns></returns>
		tMaterialParamsList& GetParams() {
			return _materialParams;
		}
		/// <summary>
		/// Generate buffer of params that ready to send on gpu shader
		/// </summary>
		/// <param name="out">- buffer to copy to</param>
		/// <returns>size of copied data</returns>
		uint32 CopyParamsToBuffer(char** out);
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
		/// <summary>
		/// Get template of this material
		/// </summary>
		/// <returns></returns>
		MaterialTemplate* GetTemplate() {
			return _template;
		}

		void Destroy();

		void Serialize(const std::string& fpath);
		void Deserialize(byte* data, uint32 size);
	};
}
