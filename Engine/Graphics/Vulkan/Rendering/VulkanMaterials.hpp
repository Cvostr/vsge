#pragma once

#include "../VulkanDescriptors.hpp"

namespace VSGE {
	class VulkanMaterial {
	public:
		VulkanDescriptorSet* _fragmentDescriptorSet;
		VulkanBuffer* _paramsBuffer;

		VulkanMaterial() {
			_fragmentDescriptorSet = nullptr;
			_paramsBuffer = new VulkanBuffer(GpuBufferType::GPU_BUFFER_TYPE_UNIFORM);
		}

		~VulkanMaterial() {
			delete _fragmentDescriptorSet;
			delete _paramsBuffer;
		}
	};
}