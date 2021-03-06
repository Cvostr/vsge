#pragma once

#include <Core/VarTypes/Base.hpp>
#include "GpuObject.hpp"
#include <string>
#include <Resources/TextureProcessor.hpp>
#include "TextureFormat.hpp"

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

namespace VSGE {

    #define TEXTURE_FLAG_IS_RENDER_TARGET 0b1
    #define TEXTURE_FLAG_IS_CUBEMAP 0b10
    #define TEXTURE_FLAG_IS_STORAGE 0b100
    #define TEXTURE_FLAG_IS_SWAPCHAIN 0b1000

	class Texture : public IGpuObject{
    protected:
        uint32 _maxWidth; //Width of best mip map
        uint32 _maxHeight; //Height of best mip map
        uint32 _mipLevels; //Count of mip levels
        uint32 _layers; //Count of layers in texture array
        uint32 _flags;

        TextureFormat _format;
    public:

        Texture();

        virtual ~Texture();

        TextureFormat GetFormat() { return _format; }
        uint32 GetMipsCount() { return _mipLevels; }
        uint32 GetLayersCount() { return _layers; }
        uint32 GetWidth() { return _maxWidth; }
        uint32 GetHeight() { return _maxHeight; }

        void SetRenderTargetFlag(bool renderTarget);
        bool IsRenderTarget();

        void SetCubemap(bool cubemap);
        bool IsCubemap();

        void SetStorage(bool storage);
        bool IsStorage();

        void SetSwapchain(bool swapchain);
        bool IsSwapchain();

        void SetFlags(uint32 flags);

        /// <summary>
        /// Destroy texture and release memory
        /// </summary>
        virtual void Destroy(){}
        /// <summary>
        /// Create texture from byte array. supported formats - DDS, PNG
        /// </summary>
        /// <param name="data">Array of bytes</param>
        /// <param name="size">Size of byte array</param>
        /// <returns>true if successful</returns>
        bool CreateFromBuffer(byte* data, uint32 size);
        /// <summary>
        /// Create texture from file. supported formats - DDS, PNG
        /// </summary>
        /// <param name="filePath">path to file</param>
        /// <returns>true if successful</returns>
        bool CreateFromFile(std::string filePath);

        bool CreateFromProcessed(const ProcessedTexture& texture); 
        /// <summary>
        /// Create blank texture with specified parameters
        /// </summary>
        /// <param name="width">- width of new texture</param>
        /// <param name="height">- height of new texture</param>
        /// <param name="format">- format of new texture</param>
        /// <param name="layers">- count of layers of new texture</param>
        virtual void Create(uint32 width, uint32 height, TextureFormat format = TextureFormat::FORMAT_RGBA, uint32 layers = 1, uint32 mipLevels = 1) = 0;
        /// <summary>
        /// Vulkan specific - change layout to SHADER_READONLY_OPTIMAL
        /// </summary>
        /// <returns></returns>
        virtual void SetReadyToUseInShaders() {}
        /// <summary>
        /// Force image view creation
        /// </summary>
        /// <returns></returns>
        virtual bool CreateImageView() = 0;
        /// <summary>
        /// Add mip map level to early created texture
        /// </summary>
        /// <param name="data">input bytes</param>
        /// <param name="size">size of input data in bytes</param>
        /// <param name="width">width of mip map level</param>
        /// <param name="height">height of mip map level</param>
        /// <param name="level">index of mip map level</param>
        virtual void AddMipLevel(byte* data, uint32 size, uint32 width, uint32 height, uint32 level, uint32 layer) = 0;
        /// <summary>
        /// Recreate texture with new sizes but with existing format and layers count
        /// </summary>
        /// <param name="width">- new width of texture</param>
        /// <param name="height">- new height of texture</param>
        virtual void Resize(uint32 width, uint32 height) = 0;
    };

    Texture* CreateTexture();
}
