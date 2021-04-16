#pragma once

#include "../Texture.hpp"
#include "VulkanMA.hpp"

namespace VSGE {
	class VulkanTexture : public Texture {
    public:

        VulkanTexture() : Texture()
        {}

        /// <summary>
        /// Destroy texture and release memory
        /// </summary>
        virtual void Destroy() {}
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