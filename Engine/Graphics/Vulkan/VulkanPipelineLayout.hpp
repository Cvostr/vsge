#pragma once

#include "../GpuObject.hpp"
#include <vulkan/vulkan.hpp>
#include "VulkanDescriptors.hpp"

namespace VSGE {
	class VulkanPipelineLayout : public IGpuObject {
	private:
		VkPipelineLayout _pipelineLayout;
		std::vector<VkPushConstantRange> _pushConstants;
		std::vector<VulkanDescriptorSet*> _descriptor_sets;
	public:

		VulkanPipelineLayout();
		VulkanPipelineLayout(const VulkanPipelineLayout&) = delete;
		~VulkanPipelineLayout();

		VkPipelineLayout GetPipelineLayout();
		void PushDescriptorSet(VulkanDescriptorSet* set);
		void AddPushConstantRange(uint32 offset, uint32 size, VkShaderStageFlags stage = VK_SHADER_STAGE_ALL_GRAPHICS);

		bool Create();
		void Destroy();
	};
}