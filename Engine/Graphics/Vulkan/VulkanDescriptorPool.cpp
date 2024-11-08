#include "VulkanDescriptors.hpp"
#include "VulkanRAPI.hpp"

using namespace VSGE;

VulkanDescriptorPool::VulkanDescriptorPool() :
    mDescriptorSetsCount(0),
    mDescriptorPool(VK_NULL_HANDLE),
    _create_free(false)
{}

VulkanDescriptorPool::~VulkanDescriptorPool() {
    Destroy();
}

VkDescriptorPool VulkanDescriptorPool::GetDescriptorPool()
{
    return mDescriptorPool;
}

VkDescriptorPoolSize makeVkDescrPoolSize(VkDescriptorType type, unsigned int descr_count) {
    VkDescriptorPoolSize poolSize{};
    poolSize.type = type;
    poolSize.descriptorCount = descr_count;
    return poolSize;
}

void VulkanDescriptorPool::SetPoolSizes(VkDescriptorPoolSize* poolSizes, uint32 poolSizesCount) {
    this->_sizes.clear();

    for (uint32 size_i = 0; size_i < poolSizesCount; size_i++) {
        _sizes.push_back(poolSizes[size_i]);
    }
}

void VulkanDescriptorPool::AddPoolSize(VkDescriptorType type, uint32 size) {
    VkDescriptorPoolSize _size = {};
    _size.descriptorCount = size;
    _size.type = type;
    _sizes.push_back(_size);
}

void VulkanDescriptorPool::SetDescriptorSetsCount(uint32 descriptorSets) {
    mDescriptorSetsCount = descriptorSets;
}

void VulkanDescriptorPool::SetCreateFreeDescrSets(bool create_free) {
    _create_free = create_free;
}

bool VulkanDescriptorPool::Create() {
    VulkanRAPI* vulkan = VulkanRAPI::Get();
    VulkanDevice* device = vulkan->GetDevice();

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.pNext = nullptr;
    poolInfo.flags = _create_free ? VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT : 0;
    poolInfo.poolSizeCount = static_cast<uint32>(_sizes.size());
    poolInfo.pPoolSizes = _sizes.data();
    poolInfo.maxSets = mDescriptorSetsCount;

    if (vkCreateDescriptorPool(device->getVkDevice(), &poolInfo, nullptr, &mDescriptorPool) != VK_SUCCESS) {
        return false;
    }

    mCreated = true;

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

void VulkanDescriptorPool::AddLayoutBinding(VkDescriptorType type, uint32 descriptorsCount) {
    VkDescriptorPoolSize poolSize = makeVkDescrPoolSize(type, descriptorsCount);
    _sizes.push_back(poolSize);
}