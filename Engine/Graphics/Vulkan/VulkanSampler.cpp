#include "VulkanSampler.hpp"
#include "VulkanRAPI.hpp"

using namespace VSGE;

VkFilter GetSamplerFilterGL(TextureFilteringMode TFM) {
	VkFilter result = VK_FILTER_NEAREST;

	if (TFM == SAMPLER_FILTERING_LINEAR)
		result = VK_FILTER_LINEAR;
	if (TFM == SAMPLER_FILTERING_NEAREST)
		result = VK_FILTER_NEAREST;

	return result;
}

VkSamplerAddressMode GetSamplerWrapMode(TextureWrapMode TWM) {
	VkSamplerAddressMode result = VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;

	if (TWM == SAMPLER_WRAP_REPEAT)
		result = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	if (TWM == SAMPLER_WRAP_MIRRORED_REPEAT)
		result = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	if (TWM == SAMPLER_WRAP_CLAMP_TO_EDGE)
		result = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

	return result;
}

bool VulkanSampler::Create() {
	VulkanRAPI* vulkan = VulkanRAPI::Get();
	VulkanDevice* device = vulkan->GetDevice();

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = GetSamplerFilterGL(mMagFiltering);
	samplerInfo.minFilter = GetSamplerFilterGL(mMinFiltering);

	samplerInfo.addressModeU = GetSamplerWrapMode(mWrapU);
	samplerInfo.addressModeV = GetSamplerWrapMode(mWrapV);
	samplerInfo.addressModeW = GetSamplerWrapMode(mWrapW);

	samplerInfo.anisotropyEnable = (mMaxAnisotropy > 1.f);
	samplerInfo.maxAnisotropy = mMaxAnisotropy;

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(device->getVkDevice(), &samplerInfo, nullptr, &this->mSampler)) {
		return false;
	}

	mCreated = true;

	return true;
}

void VulkanSampler::Destroy() {
	if (mCreated) {
		VulkanRAPI* vulkan = VulkanRAPI::Get();
		VulkanDevice* device = vulkan->GetDevice();
		vkDestroySampler(device->getVkDevice(), mSampler, nullptr);
		mCreated = false;
	}
}