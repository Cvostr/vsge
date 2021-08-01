#include "VulkanSampler.hpp"
#include "VulkanRAPI.hpp"

using namespace VSGE;

VkFilter GetSamplerFilter(TextureFilteringMode TFM) {
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
	if (TWM == SAMPLER_WRAP_CLAMP_TO_BORDER)
		result = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;

	return result;
}

VkBorderColor GetBorderColor(BorderColor bc) {
	VkBorderColor result = VK_BORDER_COLOR_MAX_ENUM;

	if (bc == BORDER_COLOR_OPAQUE_WHITE)
		result = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	if (bc == BORDER_COLOR_OPAQUE_BLACK)
		result = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
	if (bc == BORDER_COLOR_TRANSPARENT_BLACK)
		result = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;

	return result;
}

VkCompareOp GetCompareOp(CompareOp cop) {
	VkCompareOp result = VK_COMPARE_OP_NEVER;

	if (cop == COMPARE_OP_NEVER)
		result = VK_COMPARE_OP_NEVER;
	if (cop == COMPARE_OP_ALWAYS)
		result = VK_COMPARE_OP_ALWAYS;
	if (cop == COMPARE_OP_LESS)
		result = VK_COMPARE_OP_LESS;
	if (cop == COMPARE_OP_EQUAL)
		result = VK_COMPARE_OP_EQUAL;
	if (cop == COMPARE_OP_NOT_EQUAL)
		result = VK_COMPARE_OP_NOT_EQUAL;
	if (cop == COMPARE_OP_GREATER)
		result = VK_COMPARE_OP_GREATER;
	if (cop == COMPARE_OP_LESS_OR_EQUAL)
		result = VK_COMPARE_OP_LESS_OR_EQUAL;
	if (cop == COMPARE_OP_GREATER_OR_EQUAL)
		result = VK_COMPARE_OP_GREATER_OR_EQUAL;

	return result;
}

bool VulkanSampler::Create() {
	VulkanRAPI* vulkan = VulkanRAPI::Get();
	VulkanDevice* device = vulkan->GetDevice();

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = GetSamplerFilter(_magFiltering);
	samplerInfo.minFilter = GetSamplerFilter(_minFiltering);

	samplerInfo.addressModeU = GetSamplerWrapMode(_wrapU);
	samplerInfo.addressModeV = GetSamplerWrapMode(_wrapV);
	samplerInfo.addressModeW = GetSamplerWrapMode(_wrapW);

	samplerInfo.anisotropyEnable = (_maxAnisotropy > 1.f);
	samplerInfo.maxAnisotropy = _maxAnisotropy;

	samplerInfo.borderColor = GetBorderColor(_borderColor);

	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	samplerInfo.compareEnable = (_compareOp != COMPARE_OP_NEVER);
	samplerInfo.compareOp = GetCompareOp(_compareOp);

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = _mipLodBias;
	samplerInfo.minLod = _minLod;
	samplerInfo.maxLod = _maxLod;

	if (vkCreateSampler(device->getVkDevice(), &samplerInfo, nullptr, &this->_sampler)) {
		return false;
	}

	mCreated = true;

	return true;
}

void VulkanSampler::Destroy() {
	if (mCreated) {
		VulkanRAPI* vulkan = VulkanRAPI::Get();
		VulkanDevice* device = vulkan->GetDevice();
		vkDestroySampler(device->getVkDevice(), _sampler, nullptr);
		mCreated = false;
	}
}