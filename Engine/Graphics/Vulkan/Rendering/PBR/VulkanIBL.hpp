#pragma once

#include "VulkanSpecularMap.hpp"
#include "VulkanIrradianceMap.hpp"
#include "../VulkanEnvMap.hpp"
#include "../../VulkanSynchronization.hpp"

namespace VSGE {
	class VulkanIBL {
	private:
		VulkanEnvMap* _envmap;
		VulkanIrradianceMap* _irmap;
		VulkanSpecularMap* _spmap;

		VulkanCommandPool* _pool;
		VulkanCommandPool* _compute_pool;
		VulkanCommandBuffer* _env_cmdbuf;
		VulkanCommandBuffer* _maps_cmdbuf;

		VulkanSemaphore* _envmap_begin_semaphore;
		VulkanSemaphore* _envmap_end_semaphore;

		tEntityList* _entities_to_render;
		tEntityList* _particles_to_render;
		VulkanBuffer* _transforms_buffer;
		VulkanBuffer* _animations_buffer;
		VulkanBuffer* _particles_buffer;
		VulkanBuffer* _lights_buffer;

		bool _draw_entities;
		bool _update_queued;
		bool _enabled;
		bool _alternately;
		int _prev_step; //1 - spmap, 2 - irmap
	public:

		VulkanIBL();
		~VulkanIBL();

		void SetInputData(tEntityList& entities,
			tEntityList& particles,
			VulkanBuffer* transforms,
			VulkanBuffer* animations,
			VulkanBuffer* particles_buffer,
			VulkanBuffer* lights);
		void SetSpmapIrmapAlternately(bool alternately);
		void SetEnabled(bool enabled);
		void Update();

		void SetEnvmapStepsCount(uint32 steps);
		void SetIrmapStepsCount(uint32 steps);
		void SetSpmapStepsCount(uint32 steps);
		void SetDrawEntities(bool draw);

		void Create();
		void Destroy();

		void RecordCmdBufs();
		void Execute(VulkanSemaphore* end_semaphore);
		VulkanSemaphore* GetBeginSemaphore();

		VulkanTexture* GetEnvMap();
		VulkanTexture* GetIrradianceMap();
		VulkanTexture* GetSpecularMap();
		void SetScene(Scene* scene);
	};
}