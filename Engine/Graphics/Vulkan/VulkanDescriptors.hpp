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
		std::vector<VkDescriptorPoolSize> _sizes;
		uint32 mDescriptorSetsCount;
		bool _create_free;
	public:

		VulkanDescriptorPool() : 
			mDescriptorSetsCount(0),
			mDescriptorPool(VK_NULL_HANDLE),
			_create_free(false)
		{}

		~VulkanDescriptorPool() {
			Destroy();
		}

		VkDescriptorPool GetDescriptorPool() { return mDescriptorPool; }

		bool Create();
		void Destroy();

		void SetPoolSizes(VkDescriptorPoolSize* poolSizes, uint32 poolSizesCount);
		void AddPoolSize(VkDescriptorType type, uint32 size);
		void SetCreateFreeDescrSets(bool create_free);

		void SetDescriptorSetsCount(uint32 descriptorSets);

		void AddLayoutBinding(VkDescriptorType type);
		void IncDescriptorSet() { mDescriptorSetsCount++; }
	};

	class VulkanDescriptorSet : public IGpuObject {
	private:
		VkDescriptorSet mDescriptorSet;
		VkDescriptorSetLayout _layout;

		std::vector<VkDescriptorSetLayoutBinding> descriptors;
		std::map<uint32, VkDescriptorType> bindings_types;

		VulkanDescriptorPool* mDescriptorPool;
	public:

		VulkanDescriptorSet(VulkanDescriptorPool* pool) :
			mDescriptorPool(pool),
			mDescriptorSet(VK_NULL_HANDLE),
			_layout(VK_NULL_HANDLE)
		{
			pool->IncDescriptorSet();
		}

		VulkanDescriptorSet() : 
			mDescriptorSet(VK_NULL_HANDLE),
			_layout(VK_NULL_HANDLE),
			mDescriptorPool(nullptr) {}

		~VulkanDescriptorSet() {
			Destroy();
		}

		void SetDescriptorPool(VulkanDescriptorPool* pool);

		VkDescriptorSet GetDescriptorSet();

		VkDescriptorSetLayout GetDescriptorSetLayout();

		void AddDescriptor(
			VkDescriptorType type, 
			uint32 binding, 
			VkShaderStageFlags stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS, 
			uint32 descriptorsCount = 1);

		/// <summary>
		/// Bind buffer to descriptor at specified binding
		/// </summary>
		/// <param name="binding">- binding of descriptor</param>
		/// <param name="buffer">- buffer to bind to descriptor</param>
		/// <param name="offset">- offset of buffer</param>
		/// <param name="range">- size of buffer region to bind, default is whole buffer size</param>
		void WriteDescriptorBuffer(
			uint32 binding,
			VulkanBuffer* buffer,
			uint32 offset = 0,
			uint32 range = INT32_MAX);
		
		void WriteDescriptorImage(
			uint32 binding,
			VulkanTexture* texture,
			VulkanSampler* sampler = nullptr,
			VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			uint32 first_texture = 0);

		void WriteDescriptorImages(
			uint32 binding, 
			VulkanTexture** textures,
			VulkanSampler* sampler,
			uint32 textures_count = 1,
			uint32 first_texture = 0);

		bool Create();
		bool CreateLayout();
		void Destroy();

	};
}