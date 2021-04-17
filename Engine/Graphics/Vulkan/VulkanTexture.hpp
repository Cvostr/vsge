#pragma once

#include "../Texture.hpp"
#include "VulkanMA.hpp"

namespace VSGE {

    VkFormat GetFormatVK(TextureFormat format);

	class VulkanTexture : public Texture {
    private:
        VmaVkImage mImage;
        VkImageView mImageView;

        VkImageUsageFlagBits usage;
        
        bool CreateFromBufferPNG(byte* data, uint32 size) {
            return false;
        }
        bool CreateFromBufferDDS(byte* data, uint32 size) { return false; }

    public:

        VulkanTexture() : Texture()
        {}

        VkImageView GetImageView() { return mImageView; }

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
        void Create(uint32 width, uint32 height, TextureFormat format = TextureFormat::FORMAT_RGBA, uint32 layers = 1);
        /// <summary>
        /// Recreate texture with new sizes but with existing format and layers count
        /// </summary>
        /// <param name="width">- new width of texture</param>
        /// <param name="height">- new height of texture</param>
        void Resize(uint32 width, uint32 height);
	};
}