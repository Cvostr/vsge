#pragma once

#include <Resources/ResourceTypes/TextureResource.hpp>
#include <Core/VarTypes/Variant.hpp>
#include "Shader.hpp"
#include <Core/VarTypes/Guid.hpp>
#include "VertexLayout.hpp"
#include <vector>
#include "GraphicsPipeline.hpp"
#include "GpuBuffer.hpp"
#include <map>
#include <Resources/ResourceReference.hpp>
#include "MaterialTemplate.hpp"
#include "MaterialDescriptor.hpp"

namespace VSGE {

	class Material {
	private:
		tMaterialTexturesList _materialTextures;
		tMaterialParamsList _materialParams;
		MaterialTemplate* _template;

		MaterialDescriptor* _descriptors;

		MaterialTexture* GetTextureByName(const std::string& texture_name);
		MaterialParameter* GetParameterByName(const std::string& param_name);
		
	public:

		bool _templateChanged;
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
		void SetParameter(const std::string& parameter_name, Variant value);
		
		void SetDescriptors(MaterialDescriptor* descriptors);

		MaterialDescriptor* GetDescriptors();

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

		void DestroyDescriptors();

		void Serialize(const std::string& fpath);
		void Deserialize(byte* data, uint32 size);
	};
}
