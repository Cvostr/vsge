#include "VulkanUiRenderer.hpp"
#include <UI/UiLayer.hpp>

using namespace VSGE;

VulkanUiRenderer::VulkanUiRenderer() {
	_fb_width = 1280;
	_fb_height = 720;
}
VulkanUiRenderer::~VulkanUiRenderer() {
	Destroy();
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
		set->SetDescriptorPool(_descr_pool);
		set->Create();
		_descr_sets.push_back(set);
	}

	_ui_rp = new VulkanRenderPass;
	_ui_rp->SetClearSize(_fb_width, _fb_height);
	_ui_rp->PushColorAttachment(FORMAT_RGBA);
	_ui_rp->Create();

	_ui_framebuffer = new VulkanFramebuffer;
	_ui_framebuffer->SetSize(_fb_width, _fb_height);
	_ui_framebuffer->AddAttachment(FORMAT_RGBA); //Color
	_ui_framebuffer->Create(_ui_rp);

	_ui_shader = new VulkanShader;
	_ui_shader->AddShaderFromFile("ui.vert", SHADER_STAGE_VERTEX);
	_ui_shader->AddShaderFromFile("ui.frag", SHADER_STAGE_FRAGMENT);
	ShaderCache::Get()->AddShader(_ui_shader, "ui");

	VertexLayout _vertexLayout;
	_vertexLayout.AddBinding(sizeof(Vertex));
	_vertexLayout.AddItem(0, offsetof(Vertex, pos), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	_vertexLayout.AddItem(1, offsetof(Vertex, uv), VertexLayoutFormat::VL_FORMAT_RG32_SFLOAT);

	_ui_pll = new VulkanPipelineLayout;
	_ui_pll->PushDescriptorSet(_descr_sets[0]);
	_ui_pll->Create();

	_ui_pipeline = new VulkanPipeline;
	_ui_pipeline->Create(_ui_shader, _ui_rp, _vertexLayout, _ui_pll);
}
void VulkanUiRenderer::Destroy() {

}
void VulkanUiRenderer::ResizeOutput(uint32 width, uint32 height) {
	_fb_width = width;
	_fb_height = height;

	_ui_rp->SetClearSize(width, height);
	_ui_framebuffer->Resize(_fb_width, _fb_height);
}