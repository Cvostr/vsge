#pragma once

#include "../RenderingAPI.hpp"
#include "D3D12Device.hpp"
#include "D3D12SwapChain.hpp"

#ifdef _WIN32
namespace VSGE {
	class D3D12RAPI : public IRenderingAPI {
	private:

		static D3D12RAPI* _this;

		D3D12Device* device;
		D3D12SwapChain* swapchain;

	public:

		void Create(Window* window);

		void Destroy();

		

		D3D12RAPI(){
			_this = this;

			
		}

		static D3D12RAPI* Get() {
			return _this;
		}
	};
}
#endif
