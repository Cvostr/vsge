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
	public:

		VulkanGammaCorrection();
		~VulkanGammaCorrection();

		void Create();
		void Destroy();
		void SetInputTexture(Texture* input);
		void FillCommandBuffer(VulkanCommandBuffer* cmdbuf);
		void ResizeOutput(const Vec2i& new_size);
		VulkanTexture* GetOutputTexture();
	};
}