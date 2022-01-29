#include "VulkanRenderer.hpp"
#include <Resources/ResourceCache.hpp>
#include <Core/Random.hpp>

using namespace VSGE;

VulkanMaterial::VulkanMaterial() {
	_fragmentDescriptorSet = nullptr;
	_paramsBuffer = new VulkanBuffer(GpuBufferType::GPU_BUFFER_TYPE_UNIFORM);
}

VulkanMaterial::~VulkanMaterial() {
	SAFE_RELEASE(_fragmentDescriptorSet);
	SAFE_RELEASE(_paramsBuffer);
}

VulkanPipeline* VulkanRenderer::CreatePipelineFromMaterialTemplate(MaterialTemplate* mat_template) {
	VulkanRenderPass* renderpass = GetMaterialsRenderPass();
	if (mat_template->GetRenderStage() != RENDER_STAGE_GBUFFER) {
		renderpass = _main_render_target->GetDeferredLightRenderer()->GetRenderPass();
	}
	
	VulkanPipelineLayout* p_layout = new VulkanPipelineLayout;
	//Add common vertex descriptor
	p_layout->PushDescriptorSet(_main_render_target->GetGBufferRenderer()->GetVertexDescriptorSets()[0]);
	//Add materials descriptor
	VulkanDescriptorSet* materialsDescrSet = CreateDescriptorSetFromMaterialTemplate(mat_template);
	p_layout->PushDescriptorSet(materialsDescrSet);
	p_layout->PushDescriptorSet(_main_render_target->GetGBufferRenderer()->GetAnimationsDescriptorSet());
	if (mat_template->GetRenderStage() != RENDER_STAGE_GBUFFER) {
		p_layout->PushDescriptorSet(_main_render_target->GetDeferredLightRenderer()->GetDeferredDescriptorSet());
	}
	//Create pipeline layout
	p_layout->Create();

	//Create pipeline
	VulkanPipeline* pipeline = new VulkanPipeline;
	pipeline->SetDepthTest(mat_template->GetDepthTest());
	pipeline->SetCullMode(mat_template->GetCullMode());
	pipeline->SetBlendingDescs(mat_template->GetBlendingAttachmentDescs());
	pipeline->Create((VulkanShader*)mat_template->GetShader(), renderpass, mat_template->GetLayout(), p_layout);
	//Store pipeline pointer in material template	
	mat_template->SetPipeline(pipeline);

	return pipeline;
}

VulkanMaterial* VulkanRenderer::CreateVulkanMaterial(Material* material) {
	if (!material->GetTemplate())
		return nullptr;

	if (material->GetDescriptors() == nullptr) {
		VulkanMaterial* mat = new VulkanMaterial;
		//Create params buffer
		mat->_paramsBuffer->Create(1024);
		//Create descriptor from material template
		mat->_fragmentDescriptorSet = CreateDescriptorSetFromMaterialTemplate(material->GetTemplate());
		//Add buffer to descriptor
		mat->_fragmentDescriptorSet->WriteDescriptorBuffer(0, mat->_paramsBuffer, 0, 1024);
		//Store descriptor pointer in material
		material->SetDescriptors(mat);

		return mat;
	}

	return nullptr;
}

VulkanDescriptorSet* VulkanRenderer::CreateDescriptorSetFromMaterialTemplate(MaterialTemplate* mat_template) {
	VulkanDescriptorSet* set = new VulkanDescriptorSet;
	set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, VK_SHADER_STAGE_FRAGMENT_BIT);
	for (MaterialTexture* texture : mat_template->GetTextures())
	{
		set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, texture->_binding, VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	set->SetDescriptorPool(mMaterialsDescriptorPool);
	set->Create();

	return set;
}

void VulkanRenderer::UpdateMaterialDescrSet(Material* mat) {
	CreateVulkanMaterial(mat);

	VulkanMaterial* vmat = static_cast<VulkanMaterial*>(mat->GetDescriptors());

	for (MaterialTexture* tex : mat->GetTextures()) {
		TextureResource* texture_res = static_cast<TextureResource*>(tex->_resource.GetResource());
		if (texture_res == nullptr) {
			continue;
		}
		if (texture_res->IsReady()) {
			//Mark texture resource as used in this frame
			texture_res->Use();
		}else
			mat->_texturesDirty = true;
		
	}
	bool unloaded_texture = false;
	if (mat->_texturesDirty) {
		for (MaterialTexture* tex : mat->GetTextures()) {
			TextureResource* texture_res = static_cast<TextureResource*>(tex->_resource.GetResource());
			if (texture_res == nullptr) {
				//if no texture bound, then bind default white texture
				vmat->_fragmentDescriptorSet->WriteDescriptorImage(tex->_binding, mEmptyZeroTexture, this->mMaterialMapsSampler);
				continue;
			}

			if (texture_res->IsUnloaded()) {
				//Load texture
				texture_res->Load();
			}

			if (texture_res->IsReady()) {
				//Mark texture resource as used in this frame
				texture_res->Use();
				//Write texture to descriptor
				vmat->_fragmentDescriptorSet->WriteDescriptorImage(tex->_binding, (VulkanTexture*)texture_res->GetTexture(), this->mMaterialMapsSampler);
			}else
				//Not all textures are loaded, 
				//we have to return to this material in next frame
				unloaded_texture = true;
		}
		if (!unloaded_texture)
			mat->_texturesDirty = false;
	}

	if (mat->_paramsDirty) {
		char* buffer = nullptr;
		uint32 size = mat->CopyParamsToBuffer(&buffer);

		vmat->_paramsBuffer->WriteData(0, size, buffer);
		delete[] buffer;
		mat->_paramsDirty = false;
	}
}