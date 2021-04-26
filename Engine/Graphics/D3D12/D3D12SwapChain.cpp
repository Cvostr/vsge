#include "D3D12SwapChain.hpp"
#include <Engine/Window.hpp>
#include <Core/Logger.hpp>

#ifdef _WIN32

using namespace VSGE;

bool D3D12SwapChain::Create(D3D12Device* device) {
    Window* win = Window::Get();

    DXGI_MODE_DESC backBufferDesc = {}; // this is to describe our display mode
    ZeroMemory(&backBufferDesc, sizeof(DXGI_MODE_DESC));
    backBufferDesc.Width = (UINT)win->GetWindowWidth(); // buffer width
    backBufferDesc.Height = (UINT)win->GetWindowHeight(); // buffer height
    backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the buffer (rgba 32 bits, 8 bits for each chanel)
    backBufferDesc.RefreshRate.Numerator = 60;
    backBufferDesc.RefreshRate.Denominator = 1;
    backBufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    backBufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    DXGI_SAMPLE_DESC sampleDesc = {};
    sampleDesc.Count = 1; // multisample count (no multisampling, so we just put 1, since we still need 1 sample)
    sampleDesc.Quality = 0;

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
    swapChainDesc.BufferCount = 1; // number of buffers we have
    swapChainDesc.BufferDesc = backBufferDesc; // our back buffer description
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // this says the pipeline will render to this swap chain
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // dxgi will discard the buffer (data) after we call present
    swapChainDesc.OutputWindow = (HWND)win->GetHWND(); // handle to our window
    swapChainDesc.SampleDesc = sampleDesc; // our multi-sampling description
    swapChainDesc.Windowed = true; // set to true, then if in fullscreen must call SetFullScreenState with true for full screen to get uncapped fps
 
    IDXGIFactory4* dxgiFactory;
    HRESULT hr = CreateDXGIFactory2(0, __uuidof(IDXGIFactory2), (void**)&dxgiFactory);
    if (FAILED(hr))
    {
        return false;
    }

    IDXGISwapChain* tempSwapChain;

    dxgiFactory->CreateSwapChain(
        device->GetCommandQueue(), // the queue will be flushed once the swap chain is created
        &swapChainDesc, // give it the swap chain description we created above
        &tempSwapChain // store the created swap chain in a temp IDXGISwapChain interface
    );

    Logger::Log(LogType::LOG_TYPE_INFO) << "D3D12 : swapchain created!\n";

    mSwapChain = static_cast<IDXGISwapChain4*>(tempSwapChain);



    return true;
}
void D3D12SwapChain::Destroy() {
    if (mCreated) {
        mSwapChain->Release();
        mCreated = false;
    }
}

#endif