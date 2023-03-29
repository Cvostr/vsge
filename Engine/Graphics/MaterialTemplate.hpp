#pragma once

#include <Resources/ResourceTypes/TextureResource.hpp>
#include <Core/VarTypes/Variant.hpp>
#include "Shader.hpp"
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
		Variant value;

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

	typedef std::vector<MaterialTexture*> tMaterialTexturesList;
	typedef std::vector<MaterialParameter> tMaterialParamsList;

	enum MaterialRenderStage {
		RENDER_STAGE_GBUFFER,
		RENDER_STAGE_POST
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
		MaterialRenderStage _render_stage;

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
		/// <summary>
		/// Add new template parameter description
		/// </summary>
		/// <param name="name">- name of parameter field</param>
		/// <param name="baseValue"></param>
		void AddParameter(const std::string& name, Variant baseValue);
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
		/// <summary>
		/// Get current depth test parameter
		/// </summary>
		bool GetDepthTest();
		/// <summary>
		/// Set new depth test parameter
		/// </summary>
		/// <param name="depth_test">- depth test parameter</param>
		void SetDepthTest(bool depth_test);

		void SetRenderStage(MaterialRenderStage stage);

		MaterialRenderStage GetRenderStage();

		void SetVertexLayout(const VertexLayout& vertexLayout);

		void SetBlendingAttachmentDesc(uint32 attachment, const BlendAttachmentDesc& desc);

		const BlendAttachmentDescsMap& GetBlendingAttachmentDescs() {
			return _blendDescs;
		}

		VertexLayout& GetLayout() {
			return _vertexLayout;
		}
	};
}