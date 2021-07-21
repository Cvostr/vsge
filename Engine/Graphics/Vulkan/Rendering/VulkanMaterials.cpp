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