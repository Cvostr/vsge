#include "VulkanUiRenderer.hpp"
#include <UI/UiLayer.hpp>

using namespace VSGE;

VulkanUiRenderer::VulkanUiRenderer() {

}
VulkanUiRenderer::~VulkanUiRenderer() {

}

void VulkanUiRenderer::Create() {
	_ui_buffer = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
	_ui_buffer->Create(256 * MAX_UI_ELEMENTS);

	_text_uv_buffer = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
	_text_uv_buffer->Create(256 * MAX_UI_TEXT_ELEMENTS);

	_descr_pool = new VulkanDescriptorPool;
	_descr_pool->SetDescriptorSetsCount(MAX_UI_ELEMENTS);
	_descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2 * MAX_UI_ELEMENTS);
	_descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_UI_ELEMENTS);
	_descr_pool->Create();

	for (uint32 i = 0; i < MAX_UI_ELEMENTS; i++) {
		VulkanDescriptorSet* set = new VulkanDescriptorSet();
		set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, VK_SHADER_STAGE_VERTEX_BIT);
		set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
		set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, VK_SHADER_STAGE_FRAGMENT_BIT);
		_descr_sets.push_back(set);
	}

}
void VulkanUiRenderer::Destroy() {

}