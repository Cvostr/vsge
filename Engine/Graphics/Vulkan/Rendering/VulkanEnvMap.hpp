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

		tEntityList* _entities_to_render;
		tEntityList* _particles_to_render;
		VulkanBuffer* _transforms_buffer;
		VulkanBuffer* _animations_buffer;
		VulkanBuffer* _particles_buffer;
		VulkanBuffer* _lights_buffer;
		
		uint32 _cube_size;
		uint32 _steps;
		uint32 _processed;
		bool _enabled;
	public:

		VulkanEnvMap();
		~VulkanEnvMap();

		void Create();
		void Resize(uint32 new_size);
		void RecordCmdbuffer(VulkanCommandBuffer* cmdbuf);
		void CopyImagesToCubeTexture(VulkanCommandBuffer* cmdbuf);

		VulkanTexture* GetCubeTexture();
		void SetInputData(tEntityList& entities,
			tEntityList& particles,
			VulkanBuffer* transforms,
			VulkanBuffer* animations,
			VulkanBuffer* particles_buffer,
			VulkanBuffer* lights);
		void SetScene(Scene* scene);
		void SetStepsCount(uint32 steps);
		void SetDrawWorld(bool draw);
	};
}