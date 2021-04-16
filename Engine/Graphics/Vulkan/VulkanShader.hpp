#pragma once

#include "../Shader.hpp"
#include <vector>
#include <vulkan/vulkan.h>

namespace VSGE {

	class VulkanShader : public Shader {
	private:
		std::vector<VkShaderModule> mModules;
	public:


		Shader(){}
	};

}