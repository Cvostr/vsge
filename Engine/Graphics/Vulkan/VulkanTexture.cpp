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
	case TextureFormat::FORMAT_R32F:
		vk_format = VK_FORMAT_R32_SFLOAT;
		break;
	case TextureFormat::FORMAT_RG32F:
		vk_format = VK_FORMAT_R32G32_SFLOAT;
		break;
	case TextureFormat::FORMAT_RGB32F:
		vk_format = VK_FORMAT_R32G32B32_SFLOAT;
		break;
	case TextureFormat::FORMAT_RGBA32F:
		vk_format = VK_FORMAT_R32G32B32A32_SFLOAT;
		break;
	case TextureFormat::FORMAT_DEPTH_24_STENCIL_8:
		vk_format = VK_FORMAT_D24_UNORM_S8_UINT;
		break;
	case TextureFormat::FORMAT_DEPTH_32:
		vk_format = VK_FORMAT_D32_SFLOAT;
		break;
	case TextureFormat::FORMAT_DEPTH_32_STENCIL_8:
		vk_format = VK_FORMAT_D32_SFLOAT_S8_UINT;
		break;
	case FORMAT_BC1_UNORM:
		vk_format = VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
		break;
	case FORMAT_BC2_UNORM:
		vk_format = VK_FORMAT_BC2_UNORM_BLOCK;
		break;
	case FORMAT_BC3_UNORM:
		vk_format = VK_FORMAT_BC3_UNORM_BLOCK;
		break;
	}

	return vk_format;
}

TextureFormat VSGE::GetTextureFormat(VkFormat format){
	TextureFormat _format;
    switch(format){
        case VK_FORMAT_D24_UNORM_S8_UINT:
		_format = FORMAT_DEPTH_24_STENCIL_8;
		break;
	case VK_FORMAT_D32_SFLOAT:
		_format = FORMAT_DEPTH_32;
		break;
	case VK_FORMAT_D32_SFLOAT_S8_UINT:
		_format = FORMAT_DEPTH_32_STENCIL_8;
		break;
    }
	return _format;
}

void VulkanTexture::Destroy() {
	if (mCreated) {
		VulkanRAPI* vulkan = VulkanRAPI::Get();
		VulkanDevice* device = vulkan->GetDevice();
		VulkanMA* ma = vulkan->GetAllocator();

		vkDestroyImageView(device->getVkDevice(), _imageView, nullptr);
		ma->destroyImage(&_image);

		_layout = VK_IMAGE_LAYOUT_UNDEFINED;
		_usage = VK_IMAGE_USAGE_SAMPLED_BIT;
		_imageView = VK_NULL_HANDLE;

		mCreated = false;
	}
}

VkImageAspectFlagBits VulkanTexture::GetAspect() {
	//Calculate image aspect
	VkImageAspectFlagBits aspect = VK_IMAGE_ASPECT_COLOR_BIT;

	if (_format == TextureFormat::FORMAT_DEPTH_32)
		aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	if (_format == TextureFormat::FORMAT_DEPTH_24_STENCIL_8)
		aspect = (VkImageAspectFlagBits)(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);

	return aspect;
}

void VulkanTexture::Create(uint32 width, uint32 height, TextureFormat format, uint32 layers, uint32 mipLevels) {
	_maxWidth = width;
	_maxHeight = height;
	_format = format;
	_layers = layers;
	_mipLevels = mipLevels;

	//Convert format from universal to vulkan
	VkFormat TexFormat = GetFormatVK(format);
	//Calculate image layout
	VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	VkImageTiling imageTiling = VK_IMAGE_TILING_OPTIMAL;
	if (IsRenderTarget())
	{
		_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	}
	if (IsRenderTarget() && format == TextureFormat::FORMAT_DEPTH_24_STENCIL_8)
	{
		_usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	}
	if (IsRenderTarget() && format == TextureFormat::FORMAT_DEPTH_32)
	{
		_usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	}
	if (!IsRenderTarget()) {
		_usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}
	if(IsStorage()){
		_usage = VK_IMAGE_USAGE_STORAGE_BIT;
	}
	
	VkImageCreateFlags imageFlags = 0;
	if (IsCubemap()) {
		imageFlags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		layers = 6;
	}

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.flags = imageFlags;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = mipLevels;
	imageInfo.arrayLayers = layers;
	imageInfo.format = TexFormat;
	imageInfo.tiling = imageTiling;
	imageInfo.initialLayout = imageLayout;
	imageInfo.usage = _usage | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

	VulkanRAPI* rapi = VulkanRAPI::Get();
	VulkanMA* ma = rapi->GetAllocator();
	VulkanDevice* device = rapi->GetDevice();

	ma->createImage(&imageInfo, &this->_image);

	if (IsRenderTarget() && format == TextureFormat::FORMAT_DEPTH_24_STENCIL_8)
	{
		ChangeLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}
	else if (IsRenderTarget() && format == TextureFormat::FORMAT_DEPTH_32) {
		ChangeLayout(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
	}
	else if (IsRenderTarget())
	{
		ChangeLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	}
	else if(IsStorage())
	{
		ChangeLayout(VK_IMAGE_LAYOUT_GENERAL);
	}
}

void VulkanTexture::AddMipLevel(byte* data, uint32 size, uint32 width, uint32 height, uint32 level, uint32 layer) {

	if (_layout == VK_IMAGE_LAYOUT_UNDEFINED)
		ChangeLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	VulkanRAPI* rapi = VulkanRAPI::Get();
	VulkanMA* ma = rapi->GetAllocator();
	
	VmaVkBuffer temp_buf;
	void* temp_map;
	ma->allocateCpu(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, &temp_buf, size, &temp_map);
	memcpy(temp_map, data, size);
	ma->unmap(&temp_buf);
	//Copy temporary buffer to image
	Transition(temp_buf, level, layer, width, height);
	//Free temporary buffer
	ma->destroyBuffer(&temp_buf);
}

bool VulkanTexture::CreateImageView() {

	if (_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		ChangeLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	VkImageViewType ImageViewType = VK_IMAGE_VIEW_TYPE_2D;
	if (_layers > 1)
		ImageViewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
	if (IsCubemap())
		ImageViewType = VK_IMAGE_VIEW_TYPE_CUBE;
	
	VkImageViewCreateInfo textureImageViewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	textureImageViewInfo.image = _image.Image;
	textureImageViewInfo.viewType = ImageViewType;
	textureImageViewInfo.format = GetFormatVK(_format);
	textureImageViewInfo.subresourceRange.aspectMask = GetAspect();
	textureImageViewInfo.subresourceRange.baseMipLevel = 0;
	textureImageViewInfo.subresourceRange.levelCount = _mipLevels;
	textureImageViewInfo.subresourceRange.baseArrayLayer = 0;
	textureImageViewInfo.subresourceRange.layerCount = _layers;
	textureImageViewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };

	VulkanRAPI* rapi = VulkanRAPI::Get();
	VulkanDevice* device = rapi->GetDevice();

	if (vkCreateImageView(device->getVkDevice(), &textureImageViewInfo, nullptr, &_imageView) != VK_SUCCESS)
		return false;

	mCreated = true;

	return true;
}

void VulkanTexture::Transition(VmaVkBuffer& buffer, uint32 MipLevel, uint32 layer, uint32 Width, uint32 Height) {
	VulkanRAPI* rapi = VulkanRAPI::Get();
	VulkanDevice* device = rapi->GetDevice();
	VulkanMA* ma = rapi->GetAllocator();

	VkCommandBuffer cmdbuf = ma->GetSingleTimeCmdBuf();


	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(cmdbuf, &beginInfo);


	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.imageSubresource.aspectMask = GetAspect();
	region.imageSubresource.baseArrayLayer = layer;
	region.imageSubresource.layerCount = 1;
	region.imageSubresource.mipLevel = MipLevel;
	region.imageExtent.width = Width;
	region.imageExtent.height = Height;
	region.imageExtent.depth = 1;

	vkCmdCopyBufferToImage(cmdbuf, buffer.Buffer, _image.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);


	//Run command
	vkEndCommandBuffer(cmdbuf);
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdbuf;

	vkQueueSubmit(device->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(device->GetGraphicsQueue());
}

void VulkanTexture::ChangeLayout(VkImageLayout newLayout) {
	VulkanRAPI* rapi = VulkanRAPI::Get();
	VulkanDevice* device = rapi->GetDevice();
	VulkanMA* ma = rapi->GetAllocator();

	VkCommandBuffer cmdbuf = ma->GetSingleTimeCmdBuf();

	VkImageMemoryBarrier imgMemBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
	imgMemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imgMemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imgMemBarrier.subresourceRange.aspectMask = GetAspect();
	imgMemBarrier.subresourceRange.baseMipLevel = 0;
	imgMemBarrier.subresourceRange.levelCount = _mipLevels;
	imgMemBarrier.subresourceRange.baseArrayLayer = 0;
	imgMemBarrier.subresourceRange.layerCount = _layers;
	imgMemBarrier.oldLayout = _layout;
	imgMemBarrier.newLayout = newLayout;
	imgMemBarrier.image = _image.Image;
	imgMemBarrier.srcAccessMask = 0;
	imgMemBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(cmdbuf, &beginInfo);

	vkCmdPipelineBarrier(
		cmdbuf,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &imgMemBarrier);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdbuf;

	vkEndCommandBuffer(cmdbuf);

	vkQueueSubmit(device->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(device->GetGraphicsQueue());

	_layout = newLayout;
}

void VulkanTexture::Resize(uint32 width, uint32 height) {
	Destroy();
	Create(width, height, _format, _layers, _mipLevels);
}

VkImageLayout VulkanTexture::GetImageLayout(){
	return _layout;
}