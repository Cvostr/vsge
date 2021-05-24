#pragma once

#include "../Texture.hpp"
#include "VulkanMA.hpp"

namespace VSGE {

    VkFormat GetFormatVK(TextureFormat format);

	class VulkanTexture : public Texture {
    private:
        VmaVkImage _image;
        VkImageView _imageView;
        VkImageLayout _layout;

        VkImageUsageFlagBits usage;

        // NOT FULLY IMPLEMENTED
        void Transition(VmaVkBuffer& buffer, uint32 MipLevel, uint32 Width, uint32 Height);

    public:

        VulkanTexture() : 
            Texture(),
            _imageView(VK_NULL_HANDLE),
            _layout(VK_IMAGE_LAYOUT_UNDEFINED)
        {}

        ~VulkanTexture() {
            Destroy();
        }

        VkImage GetImage() { return _image.Image; }
        VkImageView GetImageView() { return _imageView; }

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
        
        void AddMipLevel(byte* data, uint32 size, uint32 width, uint32 height, uint32 level);

        /// <summary>
        /// Recreate texture with new sizes but with existing format and layers count
        /// </summary>
        /// <param name="width">- new width of texture</param>
        /// <param name="height">- new height of texture</param>
        void Resize(uint32 width, uint32 height);

        bool CreateImageView();

        void ChangeLayout(VkImageLayout newLayout);
	};
}