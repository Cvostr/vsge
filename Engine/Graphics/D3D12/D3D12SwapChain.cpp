#include "D3D12SwapChain.hpp"
#include <Engine/Window.hpp>

#ifdef _WIN32

using namespace VSGE;

bool D3D12Device::Create(IDXGIAdapter1* adapter) {
    Window* win = Window::Get();

    DXGI_MODE_DESC backBufferDesc = {}; // this is to describe our display mode
    backBufferDesc.Width = (UINT)win->GetWindowWidth(); // buffer width
    backBufferDesc.Height = (UINT)win->GetWindowHeight(); // buffer height
    backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the buffer (rgba 32 bits, 8 bits for each chanel)
    
    DXGI_SAMPLE_DESC sampleDesc = {};
    sampleDesc.Count = 1; // multisample count (no multisampling, so we just put 1, since we still need 1 sample)

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 3; // number of buffers we have
    swapChainDesc.BufferDesc = backBufferDesc; // our back buffer description
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // this says the pipeline will render to this swap chain
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // dxgi will discard the buffer (data) after we call present
    swapChainDesc.OutputWindow = (HWND)win->GetHWND(); // handle to our window
    swapChainDesc.SampleDesc = sampleDesc; // our multi-sampling description
    swapChainDesc.Windowed = true; // set to true, then if in fullscreen must call SetFullScreenState with true for full screen to get uncapped fps
    
}
void D3D12Device::Destroy() {
    if (mCreated) {
        mSwapChain->Release();
        mCreated = false;
    }
}

#endif