#pragma once

#include "../GpuBuffer.hpp"
#include "VulkanMA.hpp"

namespace VSGE {
	class VulkanBuffer : public GpuBuffer {
	private:
		VmaVkBuffer mBuffer;
	public:

		VkBuffer GetBuffer() { return mBuffer.Buffer; }

		VulkanBuffer(GpuBufferType type) {
			mType = type;
		}

		void Create(uint32 size);

		void Destroy();

		void WriteData(uint32 offset, uint32 size, void* data);
	};
}