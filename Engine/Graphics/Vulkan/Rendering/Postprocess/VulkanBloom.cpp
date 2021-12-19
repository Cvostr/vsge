#include "VulkanBloom.hpp"
#include "../VulkanRenderer.hpp"

using namespace VSGE;

VulkanBloom::VulkanBloom() {
	_output_size = Vec2i(1280, 720);
}
VulkanBloom::~VulkanBloom() {
	Destroy();
}

void VulkanBloom::Create() {
	

	

	_descr_pool = new VulkanDescriptorPool;
	_descr_pool->SetDescriptorSetsCount(3);
	_descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2);
	_descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 6);
	_descr_pool->Create();

	CreateBright();
	CreateBlur();
}

void VulkanBloom::CreateBright() {
	_bloom_bright_shader = new VulkanShader;
	_bloom_bright_shader->AddShaderFromFile("bloom_bright.comp", SHADER_STAGE_COMPUTE);

	_bright_descr_set = new VulkanDescriptorSet;
	_bright_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, VK_SHADER_STAGE_COMPUTE_BIT);
	_bright_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT);
	_bright_descr_set->SetDescriptorPool(_descr_pool);
	_bright_descr_set->Create();

	_bright_pp_layout = new VulkanPipelineLayout;
	_bright_pp_layout->PushDescriptorSet(_bright_descr_set);
	_bright_pp_layout->Create();

	_bright_pipeline = new VulkanComputePipeline;
	_bright_pipeline->Create(_bloom_bright_shader, _bright_pp_layout);

	_bright_texture = new VulkanTexture;
	_bright_texture->SetStorage(true);
	_bright_texture->Create(1280, 720, FORMAT_RGBA16F, 1, 1);
}
void VulkanBloom::CreateBlur() {
	_bloom_blur_v_shader = new VulkanShader;
	_bloom_blur_v_shader->AddShaderFromFile("bloom_blur_v.comp", SHADER_STAGE_COMPUTE);

	_bloom_blur_h_shader = new VulkanShader;
	_bloom_blur_h_shader->AddShaderFromFile("bloom_blur_h.comp", SHADER_STAGE_COMPUTE);

	_blur_1_descr_set = new VulkanDescriptorSet;
	_blur_1_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0, VK_SHADER_STAGE_COMPUTE_BIT);
	_blur_1_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT);
	_blur_1_descr_set->SetDescriptorPool(_descr_pool);
	_blur_1_descr_set->Create();

	_blur_2_descr_set = new VulkanDescriptorSet;
	_blur_2_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0, VK_SHADER_STAGE_COMPUTE_BIT);
	_blur_2_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT);
	_blur_2_descr_set->SetDescriptorPool(_descr_pool);
	_blur_2_descr_set->Create();

	_blur_pp_layout = new VulkanPipelineLayout;
	_blur_pp_layout->PushDescriptorSet(_blur_1_descr_set);
	_blur_pp_layout->Create();

	_blur_v_pipeline = new VulkanComputePipeline;
	_blur_v_pipeline->Create(_bloom_blur_v_shader, _blur_pp_layout);
	_blur_h_pipeline = new VulkanComputePipeline;
	_blur_h_pipeline->Create(_bloom_blur_h_shader, _blur_pp_layout);

	_temp_texture = new VulkanTexture;
	_temp_texture->SetStorage(true);
	_temp_texture->Create(1280, 720, FORMAT_RGBA16F);

	_blur_1_descr_set->WriteDescriptorImage(0, _bright_texture, nullptr,
		VK_IMAGE_LAYOUT_GENERAL);
	_blur_1_descr_set->WriteDescriptorImage(1, _temp_texture, nullptr,
		VK_IMAGE_LAYOUT_GENERAL);
	_blur_2_descr_set->WriteDescriptorImage(0, _temp_texture, nullptr,
		VK_IMAGE_LAYOUT_GENERAL);
	_blur_2_descr_set->WriteDescriptorImage(1, _bright_texture, nullptr,
		VK_IMAGE_LAYOUT_GENERAL);
}

void VulkanBloom::Destroy() {

}

void VulkanBloom::SetInputTexture(Texture* input) {
	PostprocessEffect::SetInputTexture(input);

	_bright_descr_set->WriteDescriptorImage(0, (VulkanTexture*)input, VulkanRenderer::Get()->GetAttachmentSampler());
	_bright_descr_set->WriteDescriptorImage(1, _bright_texture, nullptr, VK_IMAGE_LAYOUT_GENERAL);
}
void VulkanBloom::FillCommandBuffer(VulkanCommandBuffer* cmdbuf) {
	VkImageMemoryBarrier pre_barrier_in = GetImageBarrier((VulkanTexture*)_input,
		0, VK_ACCESS_SHADER_WRITE_BIT,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		VK_IMAGE_LAYOUT_GENERAL);
	VkImageMemoryBarrier post_barrier_in = GetImageBarrier((VulkanTexture*)_input, 
		VK_ACCESS_SHADER_WRITE_BIT, 0, 
		VK_IMAGE_LAYOUT_GENERAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	VkImageMemoryBarrier pre_barrier_bright = GetImageBarrier((VulkanTexture*)_bright_texture,
		0, VK_ACCESS_SHADER_WRITE_BIT, 
		VK_IMAGE_LAYOUT_UNDEFINED, 
		VK_IMAGE_LAYOUT_GENERAL);
	VkImageMemoryBarrier post_barrier_bright = GetImageBarrier((VulkanTexture*)_bright_texture,
		VK_ACCESS_SHADER_WRITE_BIT, 0,
		VK_IMAGE_LAYOUT_GENERAL, 
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	VkImageMemoryBarrier pre_barrier_temp = GetImageBarrier((VulkanTexture*)_temp_texture,
		0, VK_ACCESS_SHADER_WRITE_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_GENERAL);
	VkImageMemoryBarrier post_barrier_temp = GetImageBarrier((VulkanTexture*)_temp_texture,
		VK_ACCESS_SHADER_WRITE_BIT, 0,
		VK_IMAGE_LAYOUT_GENERAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	
	cmdbuf->ImagePipelineBarrier(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, { 
			pre_barrier_in, 
		post_barrier_bright,
		pre_barrier_temp });
	cmdbuf->BindComputePipeline(*_bright_pipeline);
	cmdbuf->BindDescriptorSets(*_bright_pp_layout, 0, 1, _bright_descr_set, 0, nullptr, VK_PIPELINE_BIND_POINT_COMPUTE);
	cmdbuf->Dispatch(_output_size.x / 16 + 1, _output_size.y / 16 + 1, 1);

	VulkanComputePipeline* pipelines[2] = { _blur_v_pipeline, _blur_h_pipeline };
	for (int i = 0; i < 2; i++) {
		cmdbuf->BindComputePipeline(*(pipelines[i]));
		int prev = 2;
		for (int j = 0; j < 3; j++) {
			VulkanDescriptorSet* set = _blur_1_descr_set;
			if (prev == 1) {
				prev = 2;
				set = _blur_2_descr_set;
			}
			else
				prev = 1;

			cmdbuf->BindDescriptorSets(*_blur_pp_layout, 0, 1, set, 0, nullptr, VK_PIPELINE_BIND_POINT_COMPUTE);
			cmdbuf->Dispatch(_output_size.x / 8 + 1, _output_size.y / 8 + 1, 1);
		}
	}

	cmdbuf->ImagePipelineBarrier(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, { 
		post_barrier_bright, 
		post_barrier_in,
		post_barrier_temp 
		});

}

void VulkanBloom::ResizeOutput(const Vec2i& new_size) {
	if (_output_size == new_size)
		return;

	_output_size = new_size;
	_bright_texture->Resize(new_size.x, new_size.y);
	_temp_texture->Resize(new_size.x, new_size.y);

	_bright_descr_set->WriteDescriptorImage(1, (VulkanTexture*)_bright_texture,
		nullptr, VK_IMAGE_LAYOUT_GENERAL);

	_blur_1_descr_set->WriteDescriptorImage(0, _bright_texture, nullptr,
		VK_IMAGE_LAYOUT_GENERAL);
	_blur_1_descr_set->WriteDescriptorImage(1, _temp_texture, nullptr,
		VK_IMAGE_LAYOUT_GENERAL);
	_blur_2_descr_set->WriteDescriptorImage(0, _temp_texture, nullptr,
		VK_IMAGE_LAYOUT_GENERAL);
	_blur_2_descr_set->WriteDescriptorImage(1, _bright_texture, nullptr,
		VK_IMAGE_LAYOUT_GENERAL);
}