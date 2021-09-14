#pragma once

#include "../Texture.hpp"
#include "VulkanMA.hpp"

namespace VSGE {

    VkFormat GetFormatVK(TextureFormat format);
    TextureFormat GetTextureFormat(VkFormat format);

    class VulkanCommandBuffer;

	class VulkanTexture : public Texture {
    private:
        VmaVkImage _image;
        VkImageView _imageView;
        VkImageLayout _layout;

        VkImageUsageFlagBits _usage;

        // NOT FULLY IMPLEMENTED
        void Transition(VmaVkBuffer& buffer, uint32 MipLevel, uint32 layer, uint32 Width, uint32 Height);

    public:

        VulkanTexture() : 
            Texture(),
            _imageView(VK_NULL_HANDLE),
            _usage(VK_IMAGE_USAGE_SAMPLED_BIT),
            _layout(VK_IMAGE_LAYOUT_UNDEFINED)
        {}

        ~VulkanTexture() {
            Destroy();
        }

        /// <summary>
        /// Get vulkan image
        /// </summary>
        /// <returns></returns>
        VkImage GetImage() { return _image.Image; }
        /// <summary>
        /// Get vulkan image view
        /// </summary>
        /// <returns></returns>
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
        /// <summary>
       /// Add mip map level to early created texture
       /// </summary>
       /// <param name="data">input bytes</param>
       /// <param name="size">size of input data in bytes</param>
       /// <param name="width">width of mip map level</param>
       /// <param name="height">height of mip map level</param>
       /// <param name="level">index of mip map level</param>
        void AddMipLevel(byte* data, uint32 size, uint32 width, uint32 height, uint32 level, uint32 layer);
        /// <summary>
        /// Recreate texture with new sizes but with existing format and layers count
        /// </summary>
        /// <param name="width">- new width of texture</param>
        /// <param name="height">- new height of texture</param>
        void Resize(uint32 width, uint32 height);
        /// <summary>
        /// Force image view creation
        /// </summary>
        /// <returns></returns>
        bool CreateImageView();
        /// <summary>
        /// Vulkan specific - change layout to SHADER_READONLY_OPTIMAL
        /// </summary>
        /// <returns></returns>
        void SetReadyToUseInShaders();

        void ChangeLayout(VkImageLayout newLayout);
        void CmdChangeLayout(VulkanCommandBuffer* cmdbuf, VkImageLayout oldLayout, VkImageLayout newLayout);
        void CmdChangeLayout(VkCommandBuffer cmdbuf, VkImageLayout oldLayout, VkImageLayout newLayout);
        void CmdCopyTexture(VulkanCommandBuffer* cmdbuf, VulkanTexture* destination, uint32 first_layer = 0, uint32 layers_count = 0);

        VkImageLayout GetImageLayout();

        VkImageAspectFlagBits GetAspect();
	};
}