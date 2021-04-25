#include "D3D12Device.hpp"

#ifdef _WIN32

using namespace VSGE;

D3D12Device* VSGE::CreatePreferredDevice() {
    IDXGIFactory4* dxgiFactory;
    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
    if (FAILED(hr))
    {
        return nullptr;
    }

    IDXGIAdapter1* adapter; // adapters are the graphics card (this includes the embedded graphics on the motherboard)

    uint32 adapterIndex = 0; // we'll start looking for directx 12  compatible graphics devices starting at index 0

    D3D12Device* device = new D3D12Device;

    //List gpu, that support d3d12
    while (dxgiFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            continue;
        }
        if (device->Create(adapter))
            break;
    }

    return device;
}

bool D3D12Device::Create(IDXGIAdapter1* adapter) {
	HRESULT hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0,
		__uuidof(ID3D12Device), (void**)&mDevice);

    if (SUCCEEDED(hr))
        mCreated = true;

	return SUCCEEDED(hr);
}
void D3D12Device::Destroy() {
    if (mCreated) {
        mDevice->Release();
        mCreated = false;
    }
}

#endif