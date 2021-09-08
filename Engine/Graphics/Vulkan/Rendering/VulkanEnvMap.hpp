#pragma once

#include "../VulkanComputePipeline.hpp"
#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>

#include "VulkanGBufferRenderer.hpp"
#include "VulkanDeferredLight.hpp"

namespace VSGE {

	struct EnvMapSide {
		VulkanGBufferRenderer* _gbuffer;
		VulkanDeferredLight* _light;

		EnvMapSide():
			_gbuffer(nullptr),
			_light(nullptr)
		{}
	};

	class VulkanEnvMap {
	private:
		VulkanTexture* _env_cube_texture;
		EnvMapSide _sides[6];

		VulkanCommandPool* _envmap_cmdpool;
		VulkanCommandBuffer* _envmap_gbuffers_cmdbuf;
		VulkanCommandBuffer* _envmap_lights_cmdbuf;

		uint32 _cube_size;
	public:

		VulkanEnvMap();
		~VulkanEnvMap();

		void Create();
		void Resize(uint32 new_size);
	};
}