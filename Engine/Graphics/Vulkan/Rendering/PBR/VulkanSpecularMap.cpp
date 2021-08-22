#include "VulkanSpecularMap.hpp"

using namespace VSGE;

void VulkanSpecularMap::Create() {
	_spmap_shader = new VulkanShader;
	_spmap_shader->AddShaderFromFile("spmap.comp", SHADER_STAGE_COMPUTE);

	_spmap_pipeline_layout = new VulkanPipelineLayout;
	//_spmap_pipeline_layout->PushDescriptorSet(_brdf_descr_set);
	_spmap_pipeline_layout->Create();

	_spmap_pipeline = new VulkanComputePipeline;
	_spmap_pipeline->Create(_spmap_shader, _spmap_pipeline_layout);

	_spmap_output_texture = new VulkanTexture;
	_spmap_output_texture->SetCubemap(true);
	_spmap_output_texture->SetStorage(true);
	_spmap_output_texture->Create(1024, 1024, FORMAT_RGBA16F, 6, 1);
	_spmap_output_texture->CreateImageView();
}