#pragma once

#ifdef _WIN32

#include <windows.h>
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_4.h>

#include <Core/VarTypes/Base.hpp>
#include "../GpuObject.hpp"

namespace VSGE {
	class D3D12SwapChain : public IGpuObject {
	private:
		IDXGISwapChain3* mSwapChain;
	public:

		bool Create();
		void Destroy();

	};
}

#endif