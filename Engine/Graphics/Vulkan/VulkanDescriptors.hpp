#pragma once

#include "../GpuObject.hpp"
#include <Core/VarTypes/Base.hpp>
#include <vulkan/vulkan.hpp>
#include <vector>
#include <map>

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

		~VulkanDescriptorPool() {
			Destroy();
		}

		VkDescriptorPool GetDescriptorPool() { return mDescriptorPool; }

		bool Create();
		void Destroy();

		void SetPoolSizes(VkDescriptorPoolSize* poolSizes, uint32 poolSizesCount);
		void AddPoolSize(VkDescriptorType type, uint32 size);

		void SetDescriptorSetsCount(uint32 descriptorSets);

		void AddLayoutBinding(VkDescriptorType type);
		void IncDescriptorSet() { mDescriptorSetsCount++; }
	};

	class VulkanDescriptorSet : public IGpuObject {
	private:
		VkDescriptorSet mDescriptorSet;
		VkDescriptorSetLayout Layout;

		std::vector<VkDescriptorSetLayoutBinding> descriptors;
		std::map<uint32, VkDescriptorType> bindings_types;

		VulkanDescriptorPool* mDescriptorPool;
	public:

		VulkanDescriptorSet(VulkanDescriptorPool* pool) :
			mDescriptorPool(pool),
			mDescriptorSet(VK_NULL_HANDLE),
			Layout(VK_NULL_HANDLE)
		{
			pool->IncDescriptorSet();
		}

		VulkanDescriptorSet() : 
			mDescriptorSet(VK_NULL_HANDLE),
			Layout(VK_NULL_HANDLE),
			mDescriptorPool(nullptr) {}

		~VulkanDescriptorSet() {
			Destroy();
		}

		void SetDescriptorPool(VulkanDescriptorPool* pool) {
			mDescriptorPool = pool;
		}

		VkDescriptorSet GetDescriptorSet() { return mDescriptorSet; }
		VkDescriptorSetLayout GetDescriptorSetLayout() { return Layout; }

		void AddDescriptor(VkDescriptorType type, uint32 binding, VkShaderStageFlags stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS);

		/// <summary>
		/// Bind buffer to descriptor at specified binding
		/// </summary>
		/// <param name="binding">- binding of descriptor</param>
		/// <param name="buffer">- buffer to bind to descriptor</param>
		/// <param name="offset">- offset of buffer</param>
		/// <param name="range">- size of buffer region to bind, default is whole buffer size</param>
		void WriteDescriptorBuffer(uint32 binding, VulkanBuffer* buffer, uint32 offset = 0, uint32 range = INT32_MAX);
		void WriteDescriptorImage(uint32 binding, VulkanTexture* texture, VulkanSampler* sampler);

		bool Create();
		void Destroy();

	};
}