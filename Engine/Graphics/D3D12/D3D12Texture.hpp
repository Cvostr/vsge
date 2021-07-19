#pragma once


#ifdef _WIN32
#include <d3d12.h>
#endif

#include "../Texture.hpp"

namespace VSGE {

#ifdef _WIN32
    DXGI_FORMAT GetFormatD3D12(TextureFormat format);
#endif

    class D3D12Texture : public Texture {
    private:
        
    public:

        D3D12Texture() : Texture()
        {}

        ~D3D12Texture() {
            Destroy();
        }

        /// <summary>
        /// Destroy texture and release memory
        /// </summary>
        void Destroy();
        /// <summary>
        /// Create blank texture with specified parameters
        /// </summary>
        /// <param name="width">- width of new texture</param>
        /// <param name="height">- height of new texture</param>
        /// <param name="format">- format of new texture</param>
        /// <param name="layers">- count of layers of new texture</param>
        void Create(uint32 width, uint32 height, TextureFormat format = TextureFormat::FORMAT_RGBA, uint32 layers = 1, uint32 mipLevels = 1);

        void AddMipLevel(byte* data, uint32 size, uint32 width, uint32 height, uint32 level, uint32 layer);

        /// <summary>
        /// Recreate texture with new sizes but with existing format and layers count
        /// </summary>
        /// <param name="width">- new width of texture</param>
        /// <param name="height">- new height of texture</param>
        void Resize(uint32 width, uint32 height);

        bool CreateImageView();
    };
}