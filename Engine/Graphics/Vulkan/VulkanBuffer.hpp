#pragma once

#include "../GpuBuffer.hpp"
#include "VulkanMA.hpp"

namespace VSGE {
	class VulkanBuffer : public GpuBuffer {
	private:
		VmaVkBuffer mBuffer;
	public:
		VulkanBuffer(GpuBufferType type) {
			mType = type;
		}

		void Create(uint32 size);

		void Destroy();
	};
}