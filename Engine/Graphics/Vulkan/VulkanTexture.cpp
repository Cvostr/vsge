#include "VulkanTexture.hpp"
#include "VulkanRAPI.hpp"

using namespace VSGE;

VkFormat VSGE::GetFormatVK(TextureFormat format) {
	VkFormat vk_format = VK_FORMAT_UNDEFINED;
	switch (format) {
	case TextureFormat::FORMAT_R:
		vk_format = VK_FORMAT_R8_UNORM;
		break;
	case TextureFormat::FORMAT_RG:
		vk_format = VK_FORMAT_R8G8_UNORM;
		break;
	case TextureFormat::FORMAT_RGB:
		vk_format = VK_FORMAT_R8G8B8_UNORM;
		break;
	case TextureFormat::FORMAT_RGBA:
		vk_format = VK_FORMAT_R8G8B8A8_UNORM;
		break;
	case TextureFormat::FORMAT_R16:
		vk_format = VK_FORMAT_R16_UNORM;
		break;
	case TextureFormat::FORMAT_RG16:
		vk_format = VK_FORMAT_R16G16_UNORM;
		break;
	case TextureFormat::FORMAT_RGB16:
		vk_format = VK_FORMAT_R16G16B16_UNORM;
		break;
	case TextureFormat::FORMAT_RGBA16:
		vk_format = VK_FORMAT_R16G16B16A16_UNORM;
		break;
	case TextureFormat::FORMAT_R16F:
		vk_format = VK_FORMAT_R16_SFLOAT;
		break;
	case TextureFormat::FORMAT_RG16F:
		vk_format = VK_FORMAT_R16G16_SFLOAT;
		break;
	case TextureFormat::FORMAT_RGB16F:
		vk_format = VK_FORMAT_R16G16B16_SFLOAT;
		break;
	case TextureFormat::FORMAT_RGBA16F:
		vk_format = VK_FORMAT_R16G16B16A16_SFLOAT;
		break;
	case TextureFormat::FORMAT_DEPTH_24_STENCIL_8:
		vk_format = VK_FORMAT_D24_UNORM_S8_UINT;
		break;
	case TextureFormat::FORMAT_DEPTH_32:
		vk_format = VK_FORMAT_D32_SFLOAT;
		break;
	}

	return vk_format;
}

void VulkanTexture::Destroy() {
	if (mCreated) {
		VulkanRAPI* vulkan = VulkanRAPI::Get();
		VulkanDevice* device = vulkan->GetDevice();

		vkDestroyImageView(device->getVkDevice(), mImageView, nullptr);

		mCreated = false;
	}
}

void VulkanTexture::Create(uint32 width, uint32 height, TextureFormat format, uint32 layers) {
	mMaxWidth = width;
	mMaxHeight = height;
	mFormat = format;
	mLayers = layers;
	mMipLevels = 1;

	//Convert format from universal to vulkan
	VkFormat TexFormat = GetFormatVK(format);
	//Calculate image layout
	VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	if (mIsRenderTarget)
	{
		imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}
	if (mIsRenderTarget && format == TextureFormat::FORMAT_DEPTH_24_STENCIL_8)
	{
		imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}
	if (mIsRenderTarget && format == TextureFormat::FORMAT_DEPTH_32)
	{
		imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
	}

	//Calculate image aspect
	VkImageAspectFlagBits aspect = VK_IMAGE_ASPECT_COLOR_BIT;

	if (format == TextureFormat::FORMAT_DEPTH_32)
		aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	if (format == TextureFormat::FORMAT_DEPTH_24_STENCIL_8)
		aspect = (VkImageAspectFlagBits)(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.flags = 0; // Optional
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = mMipLevels;
	imageInfo.arrayLayers = layers;
	imageInfo.format = TexFormat;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = imageLayout;
	imageInfo.usage = usage | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

	VulkanRAPI* rapi = VulkanRAPI::Get();
	VulkanMA* ma = rapi->GetAllocator();
	VulkanDevice* device = rapi->GetDevice();

	ma->createImage(&imageInfo, &this->mImage);

	VkImageViewType ImageViewType = VK_IMAGE_VIEW_TYPE_2D;
	if (layers > 1)
		ImageViewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;

	VkImageViewCreateInfo textureImageViewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	textureImageViewInfo.image = mImage.Image;
	textureImageViewInfo.viewType = ImageViewType;
	textureImageViewInfo.format = TexFormat;
	textureImageViewInfo.subresourceRange.aspectMask = aspect;
	textureImageViewInfo.subresourceRange.baseMipLevel = 0;
	textureImageViewInfo.subresourceRange.levelCount = 1;
	textureImageViewInfo.subresourceRange.baseArrayLayer = 0;
	textureImageViewInfo.subresourceRange.layerCount = layers;
	vkCreateImageView(device->getVkDevice(), &textureImageViewInfo, nullptr, &mImageView);
}

void VulkanTexture::Resize(uint32 width, uint32 height) {

}