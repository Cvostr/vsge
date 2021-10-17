#pragma once

#include "../../../PostprocessEffect.hpp"
#include "../../VulkanComputePipeline.hpp"
#include "../../VulkanDescriptors.hpp"
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>
#include <Math/Vec2.hpp>

namespace VSGE {
	class VulkanGammaCorrection : public PostprocessEffect {
	private:
		VulkanShader* _shader;
		
		VulkanDescriptorPool* _descr_pool;
		VulkanDescriptorSet* _descr_set;
		VulkanPipelineLayout* _pp_layout;
		VulkanComputePipeline* _pipeline;

		Vec2i _output_sizes;
	public:

		VulkanGammaCorrection();
		~VulkanGammaCorrection();

		void Create();
		void Destroy();
		void SetInputTexture(Texture* input);
		void FillCommandBuffer(VulkanCommandBuffer* cmdbuf);
		void ResizeOutput(const Vec2i& new_size);
	};
}