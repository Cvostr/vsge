#pragma once

#include <Graphics/Vulkan/VulkanShader.hpp>
#include <Graphics/Vulkan/VulkanPipeline.hpp>

namespace VSGEditor {
	class VkGizmos {
	private:
		static VkGizmos* _this;

		VSGE::VulkanShader* _object_shader;
	public:

		static VkGizmos* Get() {
			return _this;
		}

		VkGizmos();
		~VkGizmos();
	};
}