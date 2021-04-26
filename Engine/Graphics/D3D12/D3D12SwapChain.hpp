#pragma once

#ifdef _WIN32

#include <windows.h>
#undef CreateWindow
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_4.h>

#include <Core/VarTypes/Base.hpp>
#include "../GpuObject.hpp"

#include "D3D12Device.hpp"

namespace VSGE {
	class D3D12SwapChain : public IGpuObject {
	private:
		IDXGISwapChain* mSwapChain;
	public:

		bool Create(D3D12Device* device);
		void Destroy();

	};
}

#endif