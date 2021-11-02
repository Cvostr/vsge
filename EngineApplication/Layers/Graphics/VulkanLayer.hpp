#pragma once

#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include <Graphics/Vulkan/VulkanFramebuffer.hpp>
#include <Graphics/Vulkan/VulkanRenderpass.hpp>
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>
#include <Graphics/Vulkan/VulkanSynchronization.hpp>
#include <Engine/ApplicationLayer.hpp>

namespace VSGE {
	class VulkanLayer : public IApplicationLayer {
	private:
		bool _recreated;

		VulkanCommandPool* _cmdpool;
		VulkanCommandBuffer* _cmdbuf;

		VulkanShader* _copy_shader;
		VulkanRenderPass* _output_rp;
		VulkanFramebuffer* _output_fb;
		VulkanPipelineLayout* _output_pipeline_layout;
		VulkanPipeline* output_pipeline;

		VulkanDescriptorPool* _pool;
		VulkanDescriptorSet* _set;

		VulkanSemaphore* _imageAvailable;
		VulkanSemaphore* _presentBegin;

		void RecordCmdbuf();
	public:
		void OnAttach();
		void OnUpdate();
		void OnDetach();
	};
}