#pragma once

#include "../VulkanDescriptors.hpp"
#include <Graphics/MaterialDescriptor.hpp>

namespace VSGE {
	class VulkanMaterial : public MaterialDescriptor {
	public:
		VulkanDescriptorSet* _fragmentDescriptorSet;
		VulkanBuffer* _paramsBuffer;

		VulkanMaterial();

		~VulkanMaterial();
	};
}