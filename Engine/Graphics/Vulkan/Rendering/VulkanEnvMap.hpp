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
		VulkanSemaphore* _envmap_middle_semaphore;
		VulkanSemaphore* _envmap_begin_semaphore;

		tEntityList* _entities_to_render;
		tEntityList* _particles_to_render;
		VulkanBuffer* _transforms_buffer;
		VulkanBuffer* _animations_buffer;
		VulkanBuffer* _particles_buffer;
		VulkanBuffer* _lights_buffer;
		
		uint32 _cube_size;
	public:

		VulkanEnvMap();
		~VulkanEnvMap();

		void Create();
		void Resize(uint32 new_size);
		void RecordCmdbufs();
		void Execute(VulkanSemaphore* end);
		VulkanSemaphore* GetBeginSemaphore();
		void CopyImagesToCubeTexture(VulkanCommandBuffer* cmdbuf);

		void SetInputData(tEntityList& entities,
			tEntityList& particles,
			VulkanBuffer* transforms,
			VulkanBuffer* animations,
			VulkanBuffer* particles_buffer,
			VulkanBuffer* lights);
	};
}