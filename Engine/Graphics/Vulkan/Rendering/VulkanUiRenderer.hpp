#pragma once

#include <Graphics/Vulkan/VulkanBuffer.hpp>
#include <Graphics/Vulkan/VulkanDescriptors.hpp>

#define MAX_UI_ELEMENTS 300
#define MAX_UI_TEXT_ELEMENTS 150

namespace VSGE {
	class VulkanUiRenderer {
	private:
		VulkanBuffer* _ui_buffer;
		VulkanBuffer* _text_uv_buffer;
		VulkanDescriptorPool* _descr_pool;
		std::vector<VulkanDescriptorSet*> _descr_sets;
	public:

		VulkanUiRenderer();
		~VulkanUiRenderer();

		void Create();
		void Destroy();
	};
}