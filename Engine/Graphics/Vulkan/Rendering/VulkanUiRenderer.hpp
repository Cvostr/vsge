#pragma once

#include <Graphics/Vulkan/VulkanBuffer.hpp>
#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include <Graphics/Vulkan/VulkanFramebuffer.hpp>
#include <Graphics/Vulkan/VulkanRenderpass.hpp>
#include <Graphics/Vulkan/VulkanShader.hpp>
#include <Graphics/Vulkan/VulkanPipeline.hpp>
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>

#define MAX_UI_ELEMENTS 400
#define MAX_UI_TEXT_ELEMENTS 200

namespace VSGE {
	class VulkanUiRenderer {
	private:
		VulkanBuffer* _ui_buffer;
		VulkanBuffer* _text_uv_buffer;
		VulkanDescriptorPool* _descr_pool;
		std::vector<VulkanDescriptorSet*> _descr_sets;

		VulkanFramebuffer* _ui_framebuffer;
		VulkanRenderPass* _ui_rp;

		VulkanShader* _ui_shader;
		VulkanPipelineLayout* _ui_pll;
		VulkanPipeline* _ui_pipeline;

		VulkanCommandPool* _ui_cmdpool;
		VulkanCommandBuffer* _ui_cmdbuf;

		uint32 _fb_width;
		uint32 _fb_height;
	public:

		VulkanUiRenderer();
		~VulkanUiRenderer();

		void Create();
		void Destroy();
		void ResizeOutput(uint32 width, uint32 height);
	};
}