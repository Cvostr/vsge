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
		VulkanDescriptorSet* _ui_descr;

		bool _render_to_swapchain;
	public:

		VulkanFinalPass();
		~VulkanFinalPass();

		void Create();
		void Destroy();
		void SetRenderToSwapchain(bool sw);
		void SetAttachments(VulkanTexture* world, VulkanTexture* ui);
		void SetFramebuffer(VulkanFramebuffer* fb);
		void RecordCmdbuffer(VulkanCommandBuffer* cmdbuffer);
	};
}