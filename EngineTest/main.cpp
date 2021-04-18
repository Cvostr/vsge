#include <Engine/Application.hpp>
#include "Graphics/Vulkan/VulkanRAPI.hpp"
#include <Math/Vertex.hpp>
#include <Graphics/Vulkan/VulkanBuffer.hpp>
#include <Graphics/Vulkan/VulkanFramebuffer.hpp>
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>

using namespace VSGE;

Application* VSGEMain() {
	auto app = new Application;

	app->GetWindow().CreateWindow(1280, 720, "Test", SDL_WINDOW_VULKAN);

	VulkanRAPI* vk = new VulkanRAPI;
	vk->Create(&app->GetWindow());

	Vertex plane_verts[] = {
		// positions              // texture coords
		Vertex(Vec3(1.0f,  1.0f, 0.0f),   Vec2(1.0f, 1.0f),   Vec3(0, 0, 1)),   // top right
		Vertex(Vec3(1.0f, -1.0f, 0.0f),   Vec2(1.0f, 0.0f),   Vec3(0, 0, 1)),   // bottom right
		Vertex(Vec3(-1.0f, -1.0f, 0.0f),  Vec2(0.0f, 0.0f),   Vec3(0, 0, 1)),   // bottom left
		Vertex(Vec3(-1.0f,  1.0f, 0.0f),  Vec2(0.0f, 1.0f),   Vec3(0, 0, 1))   // top left
	};

	VulkanBuffer* vertBuffer = new VulkanBuffer(GpuBufferType::GPU_BUFFER_TYPE_VERTEX);
	vertBuffer->Create(sizeof(plane_verts));
	vertBuffer->WriteData(0, sizeof(plane_verts), plane_verts);

	VulkanRenderPass* rpass = new VulkanRenderPass;
	rpass->PushColorOutputAttachment();
	rpass->Create();

	VulkanFramebuffer* fb = new VulkanFramebuffer;
	fb->SetSize(1280, 720);
	fb->PushOutputAttachment(0);
	fb->Create(rpass);

	VulkanCommandPool* cmdpool = new VulkanCommandPool;
	cmdpool->Create(vk->GetDevice()->GetGraphicsQueueFamilyIndex());

	VulkanCommandBuffer* cmdbuf = new VulkanCommandBuffer;
	cmdbuf->Create(*cmdpool);


	return app;
}
