#include "VulkanLightProbeImage.hpp"
#include "../../VulkanRAPI.hpp"

using namespace VSGE;

#define IRMAP_SIZE 32
#define SIZE 256
#define MAP_FORMAT FORMAT_RGBA16F

VulkanLightProbeImage::VulkanLightProbeImage() :
    _irmap(nullptr),
    _spmap(nullptr),
    mipmaps(0),
	max_resolution(0),
	_descr_pool(nullptr),
	_irmap_descr_set(nullptr),
	_spmap_descr_set(nullptr)
{}

VulkanLightProbeImage::~VulkanLightProbeImage()
{
	Destroy();
}

void VulkanLightProbeImage::Create()
{
    _irmap = new VulkanTexture;
    _irmap->SetCubemap(true);
    _irmap->SetStorage(true);
    _irmap->Create(IRMAP_SIZE, IRMAP_SIZE, FORMAT_RGBA16F, 6, 1);

    _spmap = new VulkanTexture;
    _spmap->SetCubemap(true);
    _spmap->SetStorage(true);

	uint32 current_resolution = 1;
	while (current_resolution <= SIZE) {
		current_resolution *= 2;
		mipmaps++;
	}

    max_resolution = SIZE;

	_spmap->Create(SIZE, SIZE, MAP_FORMAT, 6, mipmaps);
}

void VulkanLightProbeImage::CreateForRendering()
{
	VulkanRAPI* rapi = VulkanRAPI::Get();
	VulkanDevice* device = rapi->GetDevice();

	// Create image view for each mipmap level to bind them to descriptor set later
	for (uint32 i = 0; i < mipmaps; i++) {

		VkImageViewCreateInfo textureImageViewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		textureImageViewInfo.image = _spmap->GetImage();
		textureImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		textureImageViewInfo.format = GetFormatVK(MAP_FORMAT);
		textureImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		textureImageViewInfo.subresourceRange.baseMipLevel = i;
		textureImageViewInfo.subresourceRange.levelCount = 1;
		textureImageViewInfo.subresourceRange.baseArrayLayer = 0;
		textureImageViewInfo.subresourceRange.layerCount = 6;
		textureImageViewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };

		VkImageView imgView;
		vkCreateImageView(device->getVkDevice(), &textureImageViewInfo, nullptr, &imgView);

		_spmapImageViews.push_back(imgView);
	}

	// ---- Descriptor pool
	_descr_pool = new VulkanDescriptorPool;
	_descr_pool->SetDescriptorSetsCount(2);
	_descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 + (mipmaps - 1));
	_descr_pool->Create();

	// IRMAP
	_irmap_descr_set = new VulkanDescriptorSet(_descr_pool);
	_irmap_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0, VK_SHADER_STAGE_COMPUTE_BIT);
	_irmap_descr_set->Create();
	// write irmap output
	_irmap_descr_set->WriteDescriptorImage(0, _irmap, nullptr, VK_IMAGE_LAYOUT_GENERAL);

	// SPMAP
	_spmap_descr_set = new VulkanDescriptorSet(_descr_pool);
	_spmap_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0, VK_SHADER_STAGE_COMPUTE_BIT, mipmaps - 1);
	_spmap_descr_set->Create();

	// We start from 1 (2nd) mip map because original image will remain
	for (uint32 i = 1; i < mipmaps; i++) {

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imageInfo.imageView = _spmapImageViews[i];
		imageInfo.sampler = nullptr;

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = _spmap_descr_set->GetDescriptorSet();
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = i - 1;

		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = nullptr;
		descriptorWrite.pImageInfo = &imageInfo; // Optional
		descriptorWrite.pTexelBufferView = nullptr; // Optional

		vkUpdateDescriptorSets(device->getVkDevice(), 1, &descriptorWrite, 0, nullptr);
	}
}

void VulkanLightProbeImage::Destroy()
{
	VulkanRAPI* vulkan = VulkanRAPI::Get();
	VulkanDevice* device = vulkan->GetDevice();

	SAFE_RELEASE(_spmap_descr_set);
	SAFE_RELEASE(_irmap_descr_set);
	SAFE_RELEASE(_descr_pool);

	for (auto& imageView : _spmapImageViews) {
		vkDestroyImageView(device->getVkDevice(), imageView, nullptr);
	}

	SAFE_RELEASE(_spmap)
	SAFE_RELEASE(_irmap)
}

VulkanDescriptorSet* VulkanLightProbeImage::GetIrmapDescrSet()
{
	return _irmap_descr_set;
}

VulkanDescriptorSet* VulkanLightProbeImage::GetSpmapDescrSet()
{
	return _spmap_descr_set;
}

Texture* VulkanLightProbeImage::GetSpmap()
{
	return _spmap;
}

Texture* VulkanLightProbeImage::GetIrmap()
{
	return _irmap;
}

uint32 VulkanLightProbeImage::GetSpMaxResolution()
{
	return max_resolution;
}

uint32 VulkanLightProbeImage::GetSpMipmapLevelsNum()
{
	return mipmaps;
}