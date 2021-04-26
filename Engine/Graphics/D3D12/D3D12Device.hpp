#pragma once

#ifdef _WIN32

#include <windows.h>
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_4.h>

#include <Core/VarTypes/Base.hpp>
#include "../GpuObject.hpp"

namespace VSGE {
	class D3D12Device : public IGpuObject {
		ID3D12Device* mDevice;
		ID3D12CommandQueue* mCommandQueue;
	public:
		bool Create(IDXGIAdapter1* adapter);
		void Destroy();

		ID3D12CommandQueue* GetCommandQueue() {
			return mCommandQueue;
		}
	};

	D3D12Device* CreatePreferredDevice();
}

#endif