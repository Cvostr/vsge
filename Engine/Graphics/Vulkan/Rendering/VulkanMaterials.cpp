#include "VulkanRenderer.hpp"

using namespace VSGE;

VulkanPipeline* VulkanRenderer::CreatePipelineFromMaterialTemplate(MaterialTemplate* mat_template) {
	VulkanPipelineLayout* p_layout = new VulkanPipelineLayout;
	//Add common vertex descriptor
	p_layout->PushDescriptorSet(mVertexDescriptorSets[0]);

	//Add materials descriptor
	VulkanDescriptorSet* materialsDescrSet = CreateDescriptorSetFromMaterialTemplate(mat_template);
	p_layout->PushDescriptorSet(materialsDescrSet);
	p_layout->PushDescriptorSet(mAnimationsDescriptorSet);
	//Create pipeline layout
	p_layout->Create();

	//Create pipeline
	VulkanPipeline* pipeline = new VulkanPipeline;
	pipeline->SetDepthTest(true);
	pipeline->SetCullMode(mat_template->GetCullMode());
	pipeline->SetBlendingDescs(mat_template->GetBlendingAttachmentDescs());
	pipeline->Create((VulkanShader*)mat_template->GetShader(), mGBufferPass, mat_template->GetLayout(), p_layout);
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
	for (MaterialTexture& texture : mat_template->GetTextures())
	{
		set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, texture._binding, VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	set->SetDescriptorPool(mMaterialsDescriptorPool);
	set->Create();

	return set;
}

void VulkanRenderer::BindMaterial(Material* mat) {
	CreateVulkanMaterial(mat);

	VulkanMaterial* vmat = static_cast<VulkanMaterial*>(mat->GetDescriptors());

	//if (mat->_texturesDirty) {
	for (MaterialTexture& tex : mat->GetTextures()) {
		TextureResource* texture_res = static_cast<TextureResource*>(tex._resource.GetResource());
		if (texture_res == nullptr) {
			//if no texture bound, then bind default white texture
			vmat->_fragmentDescriptorSet->WriteDescriptorImage(tex._binding, mEmptyTexture, this->mMaterialMapsSampler);
			continue;
		}

		if (texture_res->GetState() == RESOURCE_STATE_UNLOADED) {
			//Load texture
			texture_res->Load();
		}

		if (texture_res->GetState() == RESOURCE_STATE_READY) {
			//Mark texture resource as used in this frame
			texture_res->Use();
			//Write texture to descriptor

			vmat->_fragmentDescriptorSet->WriteDescriptorImage(tex._binding, (VulkanTexture*)texture_res->GetTexture(), this->mMaterialMapsSampler);
			mat->_texturesDirty = false;
		}
	}
	//}

	if (mat->_paramsDirty) {
		char* buffer = nullptr;
		uint32 size = mat->CopyParamsToBuffer(&buffer);

		vmat->_paramsBuffer->WriteData(0, size, buffer);
		delete[] buffer;
		mat->_paramsDirty = false;
	}
}