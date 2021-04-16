#pragma once

#include <Core/VarTypes/Base.hpp>
#include "GpuObject.hpp"

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

        FORMAT_BC1_UNORM,
        FORMAT_BC2_UNORM,
        FORMAT_BC3_UNORM,

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

        virtual bool CreateFromBufferPNG(byte* data, uint32 size) = 0;
        virtual bool CreateFromBufferDDS(byte* data, uint32 size) = 0;
    public:

        Texture() : 
            mMaxWidth(0),
            mMaxHeight(0),
            mMipLevels(0),
            mLayers(1),
            mIsRenderTarget(false)
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
        /// <summary>
        /// Create blank texture with specified parameters
        /// </summary>
        /// <param name="width">- width of new texture</param>
        /// <param name="height">- height of new texture</param>
        /// <param name="format">- format of new texture</param>
        /// <param name="layers">- count of layers of new texture</param>
        virtual void Create(uint32 width, uint32 height, TextureFormat format = TextureFormat::FORMAT_RGBA, uint32 layers = 1) = 0;
        /// <summary>
        /// Recreate texture with new sizes but with existing format and layers count
        /// </summary>
        /// <param name="width">- new width of texture</param>
        /// <param name="height">- new height of texture</param>
        virtual void Resize(uint32 width, uint32 height) = 0;
    };
}