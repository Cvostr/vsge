#include "VulkanTexture.hpp"

using namespace VSGE;

VkFormat Engine::GetFormatVK(TextureFormat format) {
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

}

void VulkanTexture::Create(uint32 width, uint32 height, TextureFormat format, uint32 layers) {
	mMaxWidth = width;
	mMaxHeight = height;
	mFormat = format;
	mLayers = layers;
	mMipLevels = 1;

	//Convert format from universal to vulkan
	VkFormat TexFormat = GetFormatVK(format);
}

void VulkanTexture::Resize(uint32 width, uint32 height) {

}