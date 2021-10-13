#include "VulkanUiRenderer.hpp"
#include <UI/UiLayer.hpp>
#include <Graphics/Vulkan/VulkanRAPI.hpp>
#include <UI/UiLayer.hpp>
#include <Math/MatrixCamera.hpp>

using namespace VSGE;

static Vertex ui_sprite_vertices[] = {
	// positions // texture coords
	Vertex(Vec3(1.0f, 1.0f, 0.0f), Vec2(1.0f, 0.0f)), // top right
	Vertex(Vec3(1.0f, 0.0f, 0.0f), Vec2(1.0f, 1.0f)), // bottom right
	Vertex(Vec3(0.0f, 0.0f, 0.0f), Vec2(0.0f, 1.0f)), // bottom left
	Vertex(Vec3(0.0f, 1.0f, 0.0f), Vec2(0.0f, 0.0f)) // top left
};
static uint32 plane_inds[] = { 0,1,2, 0,2,3 };

VulkanUiRenderer::VulkanUiRenderer() {
	_fb_width = 1280;
	_fb_height = 720;
	written_elements = 0;
}
VulkanUiRenderer::~VulkanUiRenderer() {
	Destroy();
}

void VulkanUiRenderer::Create() {
	VulkanRAPI* rapi = VulkanRAPI::Get();
	VulkanDevice* device = rapi->GetDevice();

	_transforms_buffer = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
	_transforms_buffer->Create(256 * MAX_UI_ELEMENTS);

	_frag_buffer = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
	_frag_buffer->Create(256 * MAX_UI_TEXT_ELEMENTS);

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

		set->WriteDescriptorBuffer(0, _transforms_buffer, i * 256, 256);
		set->WriteDescriptorBuffer(1, _frag_buffer, i * 256, 256);

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

	_ui_cmdpool = new VulkanCommandPool;
	_ui_cmdpool->Create(device->GetGraphicsQueueFamilyIndex());
	
	_ui_cmdbuf = new VulkanCommandBuffer;
	_ui_cmdbuf->Create(_ui_cmdpool);

	_begin_semaphore = new VulkanSemaphore;
	_begin_semaphore->Create();

	_ui_sampler = new VulkanSampler;
	_ui_sampler->Create();

	_ui_sprite_mesh = new VulkanMesh;
	_ui_sprite_mesh->SetVertexBuffer(ui_sprite_vertices, 4);
	_ui_sprite_mesh->SetIndexBuffer(plane_inds, 6);
	_ui_sprite_mesh->Create();

}
void VulkanUiRenderer::Destroy() {
	SAFE_RELEASE(_ui_sprite_mesh)
}
void VulkanUiRenderer::ResizeOutput(uint32 width, uint32 height) {
	_fb_width = width;
	_fb_height = height;

	_ui_rp->SetClearSize(width, height);
	_ui_framebuffer->Resize(_fb_width, _fb_height);

	_camera_transform = GetOrthoRH_ZeroOne(0, _fb_width, 0, _fb_height, 0, 1);
}

VulkanTexture* VulkanUiRenderer::GetOutputTexture() {
	return (VulkanTexture*)_ui_framebuffer->GetColorAttachments()[0];
}
VulkanSemaphore* VulkanUiRenderer::GetBeginSemaphore() {
	return _begin_semaphore;
}
void VulkanUiRenderer::WriteTransform(uint32 elem_id, const Mat4& transform) {
	Mat4 total_transform = transform * _camera_transform;
	_transforms_buffer->WriteData(256 * elem_id, sizeof(Mat4), &total_transform);
}
void VulkanUiRenderer::WriteElement(uint32 elem_id, const Vec2& uv_min, const Vec2& uv_max, const Color& color) {
	_frag_buffer->WriteData(256 * elem_id, sizeof(Vec3), (void*)(&color));
	_frag_buffer->WriteData(256 * elem_id + 16, sizeof(Vec3), (void*)(&uv_min));
	_frag_buffer->WriteData(256 * elem_id + 32, sizeof(Vec3), (void*)(&uv_max));
}
void VulkanUiRenderer::WriteTexture(uint32 elem_id, VulkanTexture* texture) {
	VulkanDescriptorSet* set = _descr_sets[elem_id];
	set->WriteDescriptorImage(2, texture, _ui_sampler);
}

void VulkanUiRenderer::FillBuffers() {
	UiLayer* ui_layer = UiLayer::Get();
	written_elements = 0;

	for (auto& task : ui_layer->GetRenderList()->GetTasks()) {
		if (task._type == UI_RENDER_TASK_TYPE_SPRITE) {
			if (!task._sprite->IsReady()) {
				task._sprite->Load();
				continue;
			}
			WriteTransform(written_elements, task._transform);
			WriteElement(written_elements, Vec2(0, 0), Vec2(1, 1), Color(1, 1, 1, 1));
			WriteTexture(written_elements, (VulkanTexture*)task._sprite->GetTexture());
			written_elements++;
		}
		else if (task._type == UI_RENDER_TASK_TYPE_TEXT) {
			for (uint32 i = 0; i < task._text.Length(); i++) {

			}
		}
	}

}
void VulkanUiRenderer::FillCommandBuffer() {
	_ui_rp->CmdBegin(*_ui_cmdbuf, *_ui_framebuffer);
	_ui_cmdbuf->BindPipeline(*_ui_pipeline);
	_ui_cmdbuf->SetViewport(0, 0, _fb_width, _fb_height);
	
	for (uint32 i = 0; i < written_elements; i ++) {
		_ui_cmdbuf->BindDescriptorSets(*_ui_pll, 0, 1, _descr_sets[i]);
		_ui_cmdbuf->BindMesh(*_ui_sprite_mesh);
		_ui_cmdbuf->DrawIndexed(6);
	}

	_ui_cmdbuf->EndRenderPass();
}
void VulkanUiRenderer::Execute(VulkanSemaphore* end_semaphore) {

}