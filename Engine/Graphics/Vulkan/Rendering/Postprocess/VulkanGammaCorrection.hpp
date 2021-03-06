#pragma once

#include "../../../PostprocessEffect.hpp"
#include "../../VulkanPipeline.hpp"
#include "../../VulkanDescriptors.hpp"
#include "../../VulkanFramebuffer.hpp"
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>
#include <Math/Vec2.hpp>

namespace VSGE {

	class VulkanGammaCorrection : public PostprocessEffect {
	private:
		VulkanRenderPass* _rp;
		VulkanFramebuffer* _fb;

		VulkanDescriptorPool* _pool;
		VulkanDescriptorSet* _descr_set;

		void CreateConstants();
		void CreateDescriptors();

		VulkanPipeline* _pipeline;
	public:

		VulkanGammaCorrection();
		~VulkanGammaCorrection();

		void Create();
		void Destroy();
		void SetInputTexture(VulkanTexture* input);
		void SetInputBloomTexture(VulkanTexture* input);
		void FillCommandBuffer(VulkanCommandBuffer* cmdbuf);
		void ResizeOutput(const Vec2i& new_size);
		void SetKeepAlpha(bool keepAlpha);
		VulkanTexture* GetOutputTexture();
	};
}