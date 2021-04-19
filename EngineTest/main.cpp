#include <Engine/Application.hpp>
#include "Graphics/Vulkan/VulkanRAPI.hpp"
#include <Math/Vertex.hpp>
#include <Graphics/Vulkan/VulkanBuffer.hpp>
#include <Graphics/Vulkan/VulkanFramebuffer.hpp>
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>
#include <Graphics/Vulkan/VulkanShaderCompiler.hpp>
#include <Graphics/Vulkan/VulkanPipeline.hpp>
#include <Graphics/Vulkan/VulkanSynchronization.hpp>
#include <Graphics/Vulkan/VulkanDescriptors.hpp>

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

	unsigned int plane_inds[] = { 0,1,2, 0,2,3 };

	VulkanBuffer* vertBuffer = new VulkanBuffer(GpuBufferType::GPU_BUFFER_TYPE_VERTEX);
	vertBuffer->Create(sizeof(plane_verts));
	vertBuffer->WriteData(0, sizeof(plane_verts), plane_verts);

	VulkanBuffer* indBuffer = new VulkanBuffer(GpuBufferType::GPU_BUFFER_TYPE_INDEX);
	indBuffer->Create(sizeof(plane_inds));
	indBuffer->WriteData(0, sizeof(plane_inds), plane_inds);

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

	const char* vert_source = "#version 450 core \n \
#extension GL_ARB_separate_shader_objects : enable \n \
layout(location = 0) in vec3 pos; \n \
layout(location = 1) in vec2 uv; \n \
layout(location = 2) in vec3 norm; \n \
layout (binding = 0) uniform CamMatrices{ \n \
float div; \n \
}uni; \n \
void main() { \n \
vec4 v4pos = vec4(pos * uni.div, 1.0); \n \
gl_Position = v4pos; \n \
}";

	const char* frag_source = "#version 450\n \
#extension GL_ARB_separate_shader_objects : enable \n \
layout(location = 0) out vec4 outColor;\n \
void main() {\n \
outColor = vec4(1, 1, 1, 1.0);\n \
}";

	byte* vs_spv_out = nullptr;
	byte* fs_spv_out = nullptr;
	uint32 vs_size = 0, fs_size = 0;
	CompileFromGLSL(vert_source, SHADER_STAGE_VERTEX, &vs_spv_out, vs_size);
	CompileFromGLSL(frag_source, SHADER_STAGE_FRAGMENT, &fs_spv_out, fs_size);

	VulkanShader* shader = new VulkanShader;
	shader->AddShader(vs_spv_out, vs_size, SHADER_STAGE_VERTEX);
	shader->AddShader(fs_spv_out, fs_size, SHADER_STAGE_FRAGMENT);

	VulkanPipelineConf conf = {};

	VertexLayout vl(sizeof(Vertex));
	vl.AddItem(0, offsetof(Vertex, pos), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	vl.AddItem(1, offsetof(Vertex, uv), VertexLayoutFormat::VL_FORMAT_RG32_SFLOAT);
	vl.AddItem(2, offsetof(Vertex, normal), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);

	VulkanDescriptorPool* pool = new VulkanDescriptorPool;

	VulkanDescriptorSet* set = new VulkanDescriptorSet(pool);
	set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0);


	pool->Create();

	set->Create();

	


	VulkanBuffer* uniformBuffer = new VulkanBuffer(GpuBufferType::GPU_BUFFER_TYPE_UNIFORM);
	uniformBuffer->Create(16);
	float data = 0.5f;
	uniformBuffer->WriteData(0, 4, &data);

	set->WriteDescriptorBuffer(0, uniformBuffer);


	VulkanPipelineLayout* p_layout = new VulkanPipelineLayout;
	p_layout->PushDescriptorSet(set);
	p_layout->Create();

	VulkanPipeline* pipeline = new VulkanPipeline;
	pipeline->Create(conf, *shader, *rpass, vl, *p_layout);

	cmdbuf->Begin();
	rpass->CmdBegin(*cmdbuf, *fb);
	
	cmdbuf->BindPipeline(*pipeline);
	cmdbuf->BindDescriptorSets(*p_layout, 0, 1, set);
	cmdbuf->BindVertexBuffer(*vertBuffer);
	cmdbuf->BindIndexBuffer(*indBuffer);
	cmdbuf->DrawIndexed(6);

	cmdbuf->EndRenderPass();
	cmdbuf->End();

	VulkanSemaphore imageAvailable;
	imageAvailable.Create();

	VulkanSemaphore presentBegin;
	presentBegin.Create();

	while (true) {
		uint32_t _imageIndex;
		VkResult imageResult = vkAcquireNextImageKHR(vk->GetDevice()->getVkDevice(),
			vk->GetSwapChain()->GetSwapChain(), UINT64_MAX, imageAvailable.GetSemaphore(), VK_NULL_HANDLE, &_imageIndex);

		_imageIndex = 0;

		if (imageResult == VK_ERROR_OUT_OF_DATE_KHR || imageResult == VK_SUBOPTIMAL_KHR) {
			//this->PresentInfrastructure.RecreateSwapchain();
			//_imageIndex = 0;
			//return;
		}

		VkSemaphore pres = presentBegin.GetSemaphore();

		VulkanGraphicsSubmit(*cmdbuf, imageAvailable, presentBegin);

		VulkanPresent(presentBegin, _imageIndex);

	}

	return app;
}
