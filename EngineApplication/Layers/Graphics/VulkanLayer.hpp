#pragma once

#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include <Graphics/Vulkan/VulkanFramebuffer.hpp>
#include <Graphics/Vulkan/VulkanRenderpass.hpp>
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>
#include <Graphics/Vulkan/VulkanSynchronization.hpp>
#include <Engine/ApplicationLayer.hpp>
#include <Graphics/Vulkan/VulkanPresenter.hpp>

namespace VSGE {
	class VulkanLayer : public IApplicationLayer {
	private:
		VulkanCommandPool* _cmdpool;
		VulkanCommandBuffer* _cmdbuf;

		VulkanShader* _copy_shader;
		VulkanPresenter* _presenter;
		VulkanPipelineLayout* _output_pipeline_layout;
		VulkanPipeline* output_pipeline;

		VulkanDescriptorPool* _pool;
		VulkanDescriptorSet* _set;

		VulkanSemaphore* _imageAvailable;
		VulkanSemaphore* _presentBegin;

		void UpdatePresentingTexture();
		void RecordCmdbuf(uint32 index);
	public:
		void OnAttach();
		void OnUpdate();
		void OnDetach();
	};
}