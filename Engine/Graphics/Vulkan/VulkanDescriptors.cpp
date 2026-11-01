#include "VulkanDescriptors.hpp"
#include "VulkanRAPI.hpp"

using namespace VSGE;

VulkanDescriptorSet::VulkanDescriptorSet(VulkanDescriptorPool* pool) :
    mDescriptorPool(pool),
    mDescriptorSet(VK_NULL_HANDLE),
    _layout(VK_NULL_HANDLE),
    _layoutCreateFlags(0)
{
    pool->IncDescriptorSet();
}

VulkanDescriptorSet::VulkanDescriptorSet() :
    mDescriptorSet(VK_NULL_HANDLE),
    _layout(VK_NULL_HANDLE),
    mDescriptorPool(nullptr),
    _layoutCreateFlags(0)
{}

VulkanDescriptorSet::~VulkanDescriptorSet() {
    Destroy();
}

void VulkanDescriptorSet::SetDescriptorPool(VulkanDescriptorPool* pool) {
    mDescriptorPool = pool;
}
VkDescriptorSet VulkanDescriptorSet::GetDescriptorSet() {
    return mDescriptorSet; 
}
VkDescriptorSetLayout VulkanDescriptorSet::GetDescriptorSetLayout() { 
    return _layout; 
}
void VulkanDescriptorSet::AddDescriptor(VkDescriptorType type, uint32 binding, VkShaderStageFlags stageFlags, uint32 descriptorsCount) 
{
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = type;
    layoutBinding.descriptorCount = descriptorsCount;
    layoutBinding.stageFlags = stageFlags;
    layoutBinding.pImmutableSamplers = nullptr;
    // Push description
    this->descriptors.push_back(layoutBinding);
    // Register this binding in descriptor pool
    if (mDescriptorPool) {
        mDescriptorPool->AddLayoutBinding(type, descriptorsCount);
    }

    // save binding type
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

void VulkanDescriptorSet::WriteDescriptorImage(
    uint32 binding,
    VulkanTexture* texture,
    VulkanSampler* sampler,
    VkImageLayout layout,
    uint32 first_texture)
{

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = layout;
    imageInfo.imageView = texture->GetImageView();
    if(sampler)
        imageInfo.sampler = sampler->GetSampler();

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = mDescriptorSet;
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = first_texture;

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

void VulkanDescriptorSet::WriteDescriptorImages(
    uint32 binding,
    VulkanTexture** textures, 
    VulkanSampler* sampler, 
    uint32 textures_count,
    uint32 first_texture)
{
    if (textures_count == 0)
        return;

    std::vector<VkDescriptorImageInfo> imageInfos;
    for (uint32 texture_i = 0; texture_i < textures_count; texture_i++) {
        VkDescriptorImageInfo imageInfo;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = textures[texture_i]->GetImageView();
        imageInfo.sampler = sampler->GetSampler();
        imageInfos.push_back(imageInfo);
    }

    VkDescriptorType DescrType = bindings_types.at(binding);
    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = mDescriptorSet;
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = first_texture;
    descriptorWrite.descriptorType = DescrType;
    descriptorWrite.descriptorCount = textures_count;
    descriptorWrite.pBufferInfo = nullptr;
    descriptorWrite.pImageInfo = imageInfos.data();
    descriptorWrite.pTexelBufferView = nullptr;

    VulkanRAPI* vulkan = VulkanRAPI::Get();
    VulkanDevice* device = vulkan->GetDevice();

    vkUpdateDescriptorSets(device->getVkDevice(), 1, &descriptorWrite, 0, nullptr);
}

void VulkanDescriptorSet::SetLayoutCreateFlags(VkDescriptorSetLayoutCreateFlags flags)
{
    _layoutCreateFlags = flags;
}

bool VulkanDescriptorSet::CreateLayout() 
{
    VulkanRAPI* vulkan = VulkanRAPI::Get();
    VulkanDevice* device = vulkan->GetDevice();

    VkDescriptorSetLayoutCreateInfo cr_info = {};
    cr_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    cr_info.bindingCount = static_cast<uint32>(descriptors.size());
    cr_info.pBindings = descriptors.data();
    cr_info.flags = _layoutCreateFlags;

    if (vkCreateDescriptorSetLayout(device->getVkDevice(), &cr_info, nullptr, &_layout) != VK_SUCCESS) {
        return false;
    }
    return true;
}

bool VulkanDescriptorSet::Create() {
    VulkanRAPI* vulkan = VulkanRAPI::Get();
    VulkanDevice* device = vulkan->GetDevice();

    if (!CreateLayout())
        return false;

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = mDescriptorPool->GetDescriptorPool();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &_layout;

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

        vkDestroyDescriptorSetLayout(device->getVkDevice(), _layout, nullptr);
        vkFreeDescriptorSets(device->getVkDevice(), this->mDescriptorPool->GetDescriptorPool(), 1, &this->mDescriptorSet);
        mCreated = false;
    }
}