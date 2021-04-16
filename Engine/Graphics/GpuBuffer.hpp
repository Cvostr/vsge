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

	class GpuBuffer : public IGpuObject {
	protected:
		uint32 mSize;
		GpuBufferType mType;
		byte* mCpuBuffer;
	public:
		GpuBuffer() : mType(GPU_BUFFER_TYPE_NONE) {}
		GpuBuffer(GpuBufferType type) : mType(type) {}

		virtual void Create(uint32 size) = 0;
		virtual void WriteData(uint32 offset, uint32 size, void* data) = 0;
		virtual void Destroy() = 0;
	};
}