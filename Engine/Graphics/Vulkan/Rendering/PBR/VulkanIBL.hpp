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

		VulkanSemaphore* _envmap_begin;
		VulkanSemaphore* _irmap_begin;
		VulkanSemaphore* _spmap_begin;

		tEntityList* _entities_to_render;
		tEntityList* _particles_to_render;
		VulkanBuffer* _transforms_buffer;
		VulkanBuffer* _animations_buffer;
		VulkanBuffer* _particles_buffer;
		VulkanBuffer* _lights_buffer;

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