#pragma once

#include "GpuObject.hpp"
#include <Core/VarTypes/Base.hpp>

namespace VSGE {

	enum GpuBufferType {
		GPU_BUFFER_TYPE_NONE,
		GPU_BUFFER_TYPE_VERTEX,
		GPU_BUFFER_TYPE_INDEX,
		GPU_BUFFER_TYPE_UNIFORM,
		GPU_BUFFER_TYPE_STORAGE,
		GPU_BUFFER_TYPE_DYNBUFFER
	};

	enum BufferDeviceLocation {
		LOCATION_CPU_GPU,
		LOCATION_GPU
	};

	class GpuBuffer : public IGpuObject {
	protected:
		uint32 mSize;
		GpuBufferType mType;
		byte* mCpuBuffer;
		BufferDeviceLocation mDeviceLocation;
	public:
		GpuBuffer() : 
			mType(GPU_BUFFER_TYPE_NONE),
			mSize(0),
			mCpuBuffer(nullptr),
			mDeviceLocation(LOCATION_CPU_GPU)
		{}
		GpuBuffer(GpuBufferType type) : 
			mType(type), 
			mSize(0), 
			mCpuBuffer(nullptr),
			mDeviceLocation(LOCATION_CPU_GPU)
		{}

		uint32 GetSize() { return mSize; }

		virtual void Create(uint32 size, BufferDeviceLocation location = LOCATION_CPU_GPU) = 0;
		virtual void WriteData(uint32 offset, uint32 size, void* data) = 0;
		virtual void Destroy() = 0;
	};
}