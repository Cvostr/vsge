#include "D3D12SwapChain.hpp"
#include <Engine/Window.hpp>

#ifdef _WIN32

using namespace VSGE;

bool D3D12SwapChain::Create(D3D12Device* device) {
    Window* win = Window::Get();

    DXGI_MODE_DESC backBufferDesc = {}; // this is to describe our display mode
    backBufferDesc.Width = (UINT)win->GetWindowWidth(); // buffer width
    backBufferDesc.Height = (UINT)win->GetWindowHeight(); // buffer height
    backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the buffer (rgba 32 bits, 8 bits for each chanel)
    backBufferDesc.RefreshRate.Numerator = 60;
    backBufferDesc.RefreshRate.Denominator = 0;

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
 
    IDXGIFactory2* dxgiFactory;
    HRESULT hr = CreateDXGIFactory2(0, __uuidof(IDXGIFactory2), (void**)&dxgiFactory);
    if (FAILED(hr))
    {
        return false;
    }

    dxgiFactory->CreateSwapChain(
        device->GetCommandQueue(), // the queue will be flushed once the swap chain is created
        &swapChainDesc, // give it the swap chain description we created above
        &mSwapChain // store the created swap chain in a temp IDXGISwapChain interface
    );
}
void D3D12SwapChain::Destroy() {
    if (mCreated) {
        mSwapChain->Release();
        mCreated = false;
    }
}

#endif