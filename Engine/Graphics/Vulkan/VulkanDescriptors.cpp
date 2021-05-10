#include "VulkanDescriptors.hpp"
#include "VulkanRAPI.hpp"

using namespace VSGE;

//Some Vulkan constructors
VkDescriptorPoolSize getVkDescrPoolSize(VkDescriptorType type, unsigned int descr_count) {
    VkDescriptorPoolSize poolSize{};
    poolSize.type = type;
    poolSize.descriptorCount = descr_count;
    return poolSize;
}

void VulkanDescriptorPool::SetPoolSizes(VkDescriptorPoolSize* poolSizes, uint32 poolSizesCount) {
    this->mSizes.clear();

    for (uint32 size_i = 0; size_i < poolSizesCount; size_i++) {
        mSizes.push_back(poolSizes[size_i]);
    }
}

void VulkanDescriptorPool::SetDescriptorSetsCount(uint32 descriptorSets) {
    mDescriptorSetsCount = descriptorSets;
}

bool VulkanDescriptorPool::Create() {
    VulkanRAPI* vulkan = VulkanRAPI::Get();
    VulkanDevice* device = vulkan->GetDevice();

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32>(mSizes.size());
    poolInfo.pPoolSizes = mSizes.data();
    poolInfo.maxSets = mDescriptorSetsCount;

    if (vkCreateDescriptorPool(device->getVkDevice(), &poolInfo, nullptr, &mDescriptorPool) != VK_SUCCESS) {
        return false;
    }

    return true;
}

void VulkanDescriptorPool::Destroy() {
    if (mCreated) {
        VulkanRAPI* vulkan = VulkanRAPI::Get();
        VulkanDevice* device = vulkan->GetDevice();

        vkDestroyDescriptorPool(device->getVkDevice(), mDescriptorPool, nullptr);
        mCreated = false;
    }
}

void VulkanDescriptorPool::AddLayoutBinding(VkDescriptorType type) {
    VkDescriptorPoolSize poolSize = getVkDescrPoolSize(type, 1);
    mSizes.push_back(poolSize);
}

void VulkanDescriptorSet::AddDescriptor(VkDescriptorType type, uint32 binding, VkShaderStageFlags stageFlags) {
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = binding;
    samplerLayoutBinding.descriptorType = type;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.stageFlags = stageFlags;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    //Push description
    this->descriptors.push_back(samplerLayoutBinding);
    //register this binding in descriptor pool
    mDescriptorPool->AddLayoutBinding(type);

    bindings_types.insert({binding, type});
}

void VulkanDescriptorSet::WriteDescriptorBuffer(uint32 binding, VulkanBuffer* buffer, uint32 offset, uint32 range) {
    VkDeviceSize _range = range;
    if (range == INT32_MAX)
        _range = buffer->GetSize();

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.offset = offset;
    bufferInfo.range = _range;
    bufferInfo.buffer = buffer->GetBuffer();

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = mDescriptorSet;
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = 0;

    VkDescriptorType DescrType = bindings_types.at(binding);

    descriptorWrite.descriptorType = DescrType;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;
    descriptorWrite.pImageInfo = nullptr; // Optional
    descriptorWrite.pTexelBufferView = nullptr; // Optional

    VulkanRAPI* vulkan = VulkanRAPI::Get();
    VulkanDevice* device = vulkan->GetDevice();

    vkUpdateDescriptorSets(device->getVkDevice(), 1, &descriptorWrite, 0, nullptr);
}

void VulkanDescriptorSet::WriteDescriptorImage(uint32 binding, VulkanTexture* texture, VulkanSampler* sampler) {
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = texture->GetImageView();
    imageInfo.sampler = sampler->GetSampler();

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = mDescriptorSet;
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = 0;

    VkDescriptorType DescrType = bindings_types.at(binding);

    descriptorWrite.descriptorType = DescrType;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = nullptr;
    descriptorWrite.pImageInfo = &imageInfo; // Optional
    descriptorWrite.pTexelBufferView = nullptr; // Optional

    VulkanRAPI* vulkan = VulkanRAPI::Get();
    VulkanDevice* device = vulkan->GetDevice();

    vkUpdateDescriptorSets(device->getVkDevice(), 1, &descriptorWrite, 0, nullptr);
}

bool VulkanDescriptorSet::Create() {
    VulkanRAPI* vulkan = VulkanRAPI::Get();
    VulkanDevice* device = vulkan->GetDevice();

    VkDescriptorSetLayoutCreateInfo cr_info = {};
    cr_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    cr_info.bindingCount = static_cast<uint32>(descriptors.size());
    cr_info.pBindings = descriptors.data();
    cr_info.pNext = nullptr;

    if (vkCreateDescriptorSetLayout(device->getVkDevice(), &cr_info, nullptr, &Layout) != VK_SUCCESS) {
        return false;
    }

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = mDescriptorPool->GetDescriptorPool();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &Layout;

    if (vkAllocateDescriptorSets(device->getVkDevice(), &allocInfo, &mDescriptorSet) != VK_SUCCESS) {
        return false;
    }

    mCreated = true;

    return true;
}

void VulkanDescriptorSet::Destroy() {
    if (mCreated) {
        VulkanRAPI* vulkan = VulkanRAPI::Get();
        VulkanDevice* device = vulkan->GetDevice();

        vkDestroyDescriptorSetLayout(device->getVkDevice(), Layout, nullptr);
        mCreated = false;
    }
}