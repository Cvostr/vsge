#pragma once

#include "../VulkanDescriptors.hpp"
#include <Graphics/MaterialDescriptor.hpp>

namespace VSGE {
	class VulkanMaterial : public MaterialRHI {
	public:
		VulkanDescriptorSet* _fragmentDescriptorSet;
		VkDeviceSize _virtualOffset;
		void* _virtualBlock;
		void* _virtualBlockPool;

		VulkanMaterial();
		~VulkanMaterial();
	};
}