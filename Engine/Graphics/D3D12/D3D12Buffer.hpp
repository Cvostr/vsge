#pragma once

#ifdef _WIN32

#include <d3d12.h>

#include <Core/VarTypes/Base.hpp>
#include "../GpuBuffer.hpp"

namespace VSGE {
	class D3D12Buffer : public GpuBuffer {
	private:
		ID3D12Resource* mBuffer;
	public:

		D3D12Buffer(GpuBufferType type) : GpuBuffer(type)
		{
		}

		~D3D12Buffer() {
			Destroy();
		}

		ID3D12Resource* GetBuffer() { return mBuffer; }

		void Create(uint32 size, BufferDeviceLocation location = LOCATION_CPU_GPU);

		void WriteData(uint32 offset, uint32 size, void* data);

		void Destroy();
	};
}

#endif