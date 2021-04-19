#pragma once

#include "../GpuObject.hpp"
#include <Core/VarTypes/Base.hpp>
#include <vulkan/vulkan.hpp>
#include <vector>

#include "VulkanBuffer.hpp"
#include "VulkanTexture.hpp"
#include "VulkanSampler.hpp"

namespace VSGE {
	class VulkanDescriptorPool : public IGpuObject {
	private:
		VkDescriptorPool mDescriptorPool;
		std::vector<VkDescriptorPoolSize> mSizes;
		uint32 mDescriptorSetsCount;
	public:

		VulkanDescriptorPool() : 
			mDescriptorSetsCount(0),
			mDescriptorPool(VK_NULL_HANDLE)
		{}

		VkDescriptorPool GetDescriptorPool() { return mDescriptorPool; }

		bool Create();
		void Destroy();

		void AddLayoutBinding(VkDescriptorType type);
		void IncDescriptorSet() { mDescriptorSetsCount++; }
	};

	class VulkanDescriptorSet : public IGpuObject {
	private:
		VkDescriptorSet mDescriptorSet;
		VkDescriptorSetLayout Layout;

		std::vector<VkDescriptorSetLayoutBinding> descriptors;

		VulkanDescriptorPool* pool_ptr;

	public:

		VulkanDescriptorSet() : 
			mDescriptorSet(VK_NULL_HANDLE),
			Layout(VK_NULL_HANDLE),
			pool_ptr(nullptr) {}

		VkDescriptorSet GetDescriptorSet() { return mDescriptorSet; }
		VkDescriptorSetLayout GetDescriptorSetLayout() { return Layout; }

		void AddDescriptor(VkDescriptorType type, uint32 binding, VkShaderStageFlags stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS);

		void WriteDescriptorBuffer(uint32 binding, VulkanBuffer* buffer);
		void WriteDescriptorImage(uint32 binding, VulkanTexture* texture, VulkanSampler* sampler);

		bool Create();
		void Destroy();

		VulkanDescriptorSet(VulkanDescriptorPool* pool) :
			pool_ptr(pool),
			mDescriptorSet(VK_NULL_HANDLE),
			Layout(VK_NULL_HANDLE)
		{  
			pool->IncDescriptorSet();
		}
	};
}