#include "vulkan_rect_example.hpp"

#include <Engine/Application.hpp>

#include "Graphics/Vulkan/VulkanRAPI.hpp"

#include <Math/Vertex.hpp>

#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>

#include <Graphics/Vulkan/VulkanSynchronization.hpp>

#include <Core/FileLoader.hpp>


using namespace VSGETest;
using namespace VSGE;

void VulkanRectTestLayer::OnAttach() {

	Application* app = Application::Get();

	VulkanRAPI* vk = new VulkanRAPI;
	vk->Create(&app->GetWindow());

	Vertex plane_verts[] = {
		// positions              // texture coords
		Vertex(Vec3(1.0f,  1.0f, 0.0f),   Vec2(1.0f, 1.0f),   Vec3(0, 0, 1)),   // top right
		Vertex(Vec3(1.0f, -1.0f, 0.0f),   Vec2(1.0f, 0.0f),   Vec3(0, 0, 1)),   // bottom right
		Vertex(Vec3(-1.0f, -1.0f, 0.0f),  Vec2(0.0f, 0.0f),   Vec3(0, 0, 1)),   // bottom left
		Vertex(Vec3(-1.0f,  1.0f, 0.0f),  Vec2(0.0f, 1.0f),   Vec3(0, 0, 1))   // top left
	};

	unsigned int plane_inds[] = { 0,1,2, 0,2,3 };

	mesh = new VulkanMesh;
	mesh->SetVertexBuffer(plane_verts, 4);
	mesh->SetIndexBuffer(plane_inds, 6);
	mesh->Create();

	rpass = new VulkanRenderPass;
	rpass->PushColorOutputAttachment();
	rpass->Create();

	fb = new VulkanFramebuffer;
	fb->SetSize(1280, 720);
	fb->PushOutputAttachment(0);
	fb->Create(rpass);

	VulkanCommandPool* cmdpool = new VulkanCommandPool;
	cmdpool->Create(vk->GetDevice()->GetGraphicsQueueFamilyIndex());

	cmdbuf = new VulkanCommandBuffer;
	cmdbuf->Create(cmdpool);

	ShaderStorage::Get()->LoadShaderBundle("shaders/shaders.bundle", "shaders/shaders.map");

	VulkanShader* shader = new VulkanShader;
	shader->AddShaderFromFile("glsl_test.vert", SHADER_STAGE_VERTEX);
	shader->AddShaderFromFile("glsl_test.frag", SHADER_STAGE_FRAGMENT);

	VulkanPipelineConf conf = {};

	VertexLayout vl;
	vl.AddBinding(sizeof(Vertex));
	vl.AddItem(0, offsetof(Vertex, pos), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	vl.AddItem(1, offsetof(Vertex, uv), VertexLayoutFormat::VL_FORMAT_RG32_SFLOAT);
	vl.AddItem(2, offsetof(Vertex, normal), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);

	VulkanDescriptorPool* pool = new VulkanDescriptorPool;

	set = new VulkanDescriptorSet(pool);
	set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0);
	set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
	set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2);

	pool->Create();
	set->Create();

	test_texture.CreateFromFile("res/test_pic.png");
	test_texture_bc.CreateFromFile("res/test_bc.dds");

	sampler.Create();

	uniformBuffer = new VulkanBuffer(GpuBufferType::GPU_BUFFER_TYPE_UNIFORM);
	uniformBuffer->Create(16);
	scale_factor = 0.5f;
	uniformBuffer->WriteData(0, 4, &scale_factor);

	set->WriteDescriptorBuffer(0, uniformBuffer);
	set->WriteDescriptorImage(1, &test_texture, &sampler);
	set->WriteDescriptorImage(2, &test_texture_bc, &sampler);


	p_layout = new VulkanPipelineLayout;
	p_layout->PushDescriptorSet(set);
	p_layout->Create();

	pipeline = new VulkanPipeline;
	pipeline->Create(conf, *shader, *rpass, vl, *p_layout);

	
	RecordCmdbuf();
	
	imageAvailable.Create();

	
	presentBegin.Create();
}

void VulkanRectTestLayer::RecordCmdbuf() {
	cmdbuf->Begin();
	rpass->CmdBegin(*cmdbuf, *fb);

	Window* win = Window::Get();

	cmdbuf->BindPipeline(*pipeline);
	cmdbuf->SetViewport(0, 0, win->GetWindowWidth(), win->GetWindowHeight());
	cmdbuf->BindDescriptorSets(*p_layout, 0, 1, set);
	cmdbuf->BindMesh(*mesh);
	cmdbuf->DrawIndexed(6);
	cmdbuf->EndRenderPass();
	cmdbuf->End();
}

void VulkanRectTestLayer::OnUpdate() {
	VulkanRAPI* vk = VulkanRAPI::Get();
	Window* win = &Application::Get()->GetWindow();

	uint32_t _imageIndex;
	VkResult imageResult = vkAcquireNextImageKHR(vk->GetDevice()->getVkDevice(),
		vk->GetSwapChain()->GetSwapChain(), UINT64_MAX, imageAvailable.GetSemaphore(), VK_NULL_HANDLE, &_imageIndex);

	_imageIndex = 0;

	if (imageResult == VK_ERROR_OUT_OF_DATE_KHR || imageResult == VK_SUBOPTIMAL_KHR) {
		vkDeviceWaitIdle(vk->GetDevice()->getVkDevice());

		Window* win = Window::Get();
		fb->Destroy();
		fb->SetSize(win->GetWindowWidth(), win->GetWindowHeight());
		rpass->SetClearSize(win->GetWindowWidth(), win->GetWindowHeight());

		vk->GetSwapChain()->Destroy();
		vk->GetSwapChain()->initSwapchain(vk->GetDevice());

		fb->PushOutputAttachment(0);
		fb->Create(rpass);

		RecordCmdbuf();

	}

	VulkanGraphicsSubmit(*cmdbuf, imageAvailable, presentBegin);

	VulkanPresent(presentBegin, _imageIndex);
}

void VulkanRectTestLayer::OnWindowEvent(IWindowEvent& event) {
	if (event.type == EventType::EventWindowClose) {
		Application::Get()->Stop();
		//Window::Get()->SetWindowSize(1920, 1080);
	}
	if (event.type == EventType::EventMouseScrolled) {
		scale_factor += ((EventMouseScrolled*)&event)->yOffset / 100.f;
		uniformBuffer->WriteData(0, 4, &scale_factor);
	}
	if (event.type == EventType::EventMouseButtonDown) {
	}
}

void VulkanRectTestLayer::OnDetach() {

}