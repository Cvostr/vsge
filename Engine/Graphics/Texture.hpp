#pragma once

#include <Core/VarTypes/Base.hpp>
#include "GpuObject.hpp"
#include <string>

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

namespace VSGE {

    enum TextureFormat {
        FORMAT_R = 1,
        FORMAT_RG,
        FORMAT_RGB,
        FORMAT_RGBA,

        FORMAT_R16,
        FORMAT_RG16,
        FORMAT_RGB16,
        FORMAT_RGBA16,

        FORMAT_R16F,
        FORMAT_RG16F,
        FORMAT_RGB16F,
        FORMAT_RGBA16F,

        FORMAT_BC1_UNORM = 0x31545844,
        FORMAT_BC2_UNORM = 0x33545844,
        FORMAT_BC3_UNORM = 0x35545844,

        FORMAT_DEPTH_24_STENCIL_8 = 120,
        FORMAT_DEPTH_32
    };

	class Texture : public IGpuObject{
    protected:
        uint32 mMaxWidth; //Width of best mip map
        uint32 mMaxHeight; //Height of best mip map
        uint32 mMipLevels; //Count of mip levels
        uint32 mLayers; //Count of layers in texture array
        bool mIsRenderTarget;

        TextureFormat mFormat;
    public:

        Texture() : 
            mMaxWidth(0),
            mMaxHeight(0),
            mMipLevels(0),
            mLayers(1),
            mIsRenderTarget(false),
            mFormat(FORMAT_RGBA)
        {}

        TextureFormat GetFormat() { return mFormat; }
        uint32 GetMipsCount() { return mMipLevels; }
        uint32 GetLayersCount() { return mLayers; }
        uint32 GetWidth() { return mMaxWidth; }
        uint32 GetHeight() { return mMaxHeight; }
        void SetRenderTargetFlag(bool renderTarget) {
            mIsRenderTarget = renderTarget;
        }
        bool IsRenderTarget() { return mIsRenderTarget; }

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
        bool CreateFromFile(std::string filePath);
        /// <summary>
        /// Create blank texture with specified parameters
        /// </summary>
        /// <param name="width">- width of new texture</param>
        /// <param name="height">- height of new texture</param>
        /// <param name="format">- format of new texture</param>
        /// <param name="layers">- count of layers of new texture</param>
        virtual void Create(uint32 width, uint32 height, TextureFormat format = TextureFormat::FORMAT_RGBA, uint32 layers = 1, uint32 mipLevels = 1) = 0;
        /// <summary>
        /// Finish texture creation and prepare it to work by creating ImageView
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
        virtual void AddMipLevel(byte* data, uint32 size, uint32 width, uint32 height, uint32 level) = 0;
        /// <summary>
        /// Recreate texture with new sizes but with existing format and layers count
        /// </summary>
        /// <param name="width">- new width of texture</param>
        /// <param name="height">- new height of texture</param>
        virtual void Resize(uint32 width, uint32 height) = 0;
    };
}