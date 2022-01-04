#pragma once

#include <Graphics/Vulkan/VulkanRenderpass.hpp>
#include <Graphics/Vulkan/VulkanFramebuffer.hpp>
#include <Graphics/Vulkan/VulkanTexture.hpp>
#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include <Graphics/Vulkan/VulkanPipeline.hpp>

namespace VSGE {
	class VulkanFinalPass {
	private:
		VulkanRenderPass* _rp;
		VulkanFramebuffer* _fb;
		VulkanShader* _copy_shader;
		VulkanPipelineLayout* _copy_pl;
		VulkanPipeline* _copy_pipeline;

		VulkanTexture* _world_attachment;
		VulkanTexture* _ui_attachment;

		VulkanDescriptorPool* _pool;
		VulkanDescriptorSet* _world_descr;

		bool _render_to_swapchain;
		bool _draw_ui;

		uint32 _width;
		uint32 _height;
	public:

		VulkanFinalPass();
		~VulkanFinalPass();

		void Create();
		void Destroy();
		void CreateFramebuffer();
		void SetRenderToSwapchain(bool sw);
		void SetAttachments(VulkanTexture* world, VulkanTexture* ui);
		void SetFramebuffer(VulkanFramebuffer* fb);
		VulkanFramebuffer* GetFramebuffer();
		void RecordCmdbuffer(VulkanCommandBuffer* cmdbuffer);
		void Resize(uint32 width, uint32 height);
	};
}