#pragma once

#include "../VulkanDescriptors.hpp"

namespace VSGE {
	class VulkanMaterial {
	public:
		VulkanDescriptorSet* _fragmentDescriptorSet;
		VulkanBuffer* _paramsBuffer;

		VulkanMaterial();

		~VulkanMaterial();
	};
}