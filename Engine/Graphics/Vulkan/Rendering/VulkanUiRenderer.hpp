#pragma once

#include <Graphics/Vulkan/VulkanBuffer.hpp>
#include <Graphics/Vulkan/VulkanDescriptors.hpp>

namespace VSGE {
	class VulkanUiRenderer {
	private:
		VulkanBuffer* _transforms_buffer;
		VulkanDescriptorPool* _descr_pool;
		std::vector<VulkanDescriptorSet*> _descr_sets;
	public:

		VulkanUiRenderer();
		~VulkanUiRenderer();

		void Create();
		void Destroy();
	};
}