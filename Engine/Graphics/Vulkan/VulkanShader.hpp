#pragma once

#include "../Shader.hpp"
#include <vector>
#include <vulkan/vulkan.h>

namespace VSGE {

	typedef std::pair<VkShaderModule, ShaderStagesBits> VulkanShaderPair;

	VkShaderStageFlagBits GetShaderStageVK(ShaderStagesBits stage);

	class VulkanShader : public Shader {
	private:
		std::vector<VulkanShaderPair> mModules;
	public:

		void AddShader(byte* shaderData, uint32 shaderSize, ShaderStagesBits type);
		uint32 GetModulesCount() { return static_cast<uint32>(mModules.size()); }
		VulkanShaderPair* GetModules() { return mModules.data(); }

		void Destroy();

		VulkanShader(){}
	};

}