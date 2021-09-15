#pragma once

#include <Engine/ApplicationLayer.hpp>
#include <Graphics/Vulkan/VulkanSynchronization.hpp>
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>
#include <Graphics/Vulkan/VulkanTexture.hpp>
#include <Graphics/Vulkan/VulkanSampler.hpp>
#include <Graphics/Vulkan/VulkanBuffer.hpp>
#include <Graphics/Vulkan/VulkanFramebuffer.hpp>

#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include <Graphics/Vulkan/VulkanPipeline.hpp>
#include <Graphics/Vulkan/VulkanBuffer.hpp>
#include <Graphics/Vulkan/VulkanMesh.hpp>
#include <Engine/WindowEvent.hpp>

namespace VSGETest {

	class VulkanRectTestLayer : public VSGE::IApplicationLayer {
	private:
		VSGE::VulkanRenderPass* rpass;
		VSGE::VulkanFramebuffer* fb;

		VSGE::VulkanSemaphore imageAvailable;
		VSGE::VulkanSemaphore presentBegin;
		VSGE::VulkanCommandBuffer* cmdbuf;

		VSGE::VulkanTexture test_texture;
		VSGE::VulkanTexture test_texture_bc;
		VSGE::VulkanSampler sampler;

		VSGE::VulkanBuffer* uniformBuffer;
		float scale_factor;

		VSGE::VulkanPipeline* pipeline;
		VSGE::VulkanPipelineLayout* p_layout;
		VSGE::VulkanDescriptorSet* set;

		VSGE::VulkanMesh* mesh;
	public:

		VulkanRectTestLayer() {
			
		}

		void RecordCmdbuf();

		void OnAttach();
		void OnUpdate();
		void OnDetach();
		void OnEvent(const VSGE::IEvent& event);

		void OnWinClose(const VSGE::EventWindowClose& event);
		void OnScroll(const VSGE::EventMouseScrolled& event);
	};
}