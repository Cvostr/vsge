#include "D3D12RAPI.hpp"
#include "Engine/Application.hpp"

using namespace VSGE;

D3D12RAPI* D3D12RAPI::_this = 0;

void D3D12RAPI::Create(Window* window) {
	Application* app = Application::Get();

	device = CreatePreferredDevice();

	swapchain = new D3D12SwapChain;
	swapchain->Create(device);
}

void D3D12RAPI::Destroy() {

}