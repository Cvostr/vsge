#pragma once

#include "VulkanGammaCorrection.hpp"
#include "VulkanBloom.hpp"
#include "VulkanSSAO.hpp"
#include "../../VulkanCommandBuffer.hpp"
#include <Graphics/Vulkan/VulkanSynchronization.hpp>

namespace VSGE {

	class PostprocessSettings {
	public:
		bool _use_bloom;
		bool _use_ssao;

		PostprocessSettings() {
			_use_bloom = true;
			_use_ssao = true;
		}
	};

	class VulkanPostprocessing {
	private:
		Texture* _input_texture;
		Texture* _input_depth;
		Texture* _input_normals;
		Texture* _input_positions;
		Texture* _input_ui;
		PostprocessSettings _settings;
		VulkanTexture* _output_texture;

		VulkanGammaCorrection* _gamma_correction;
		VulkanBloom* _bloom;
		VulkanSSAO* _ssao;

		VulkanCommandPool* _cmdpool;
		VulkanCommandBuffer* _cmdbuf;
		VulkanSemaphore* _begin_semaphore;

		VulkanShader* _ui_add_shader;
		VulkanDescriptorPool* _ui_descr_pool;
		VulkanDescriptorSet* _ui_descr_set;
		VulkanPipelineLayout* _ui_add_pl_layout;
		VulkanComputePipeline* _ui_add_pipeline;

		Vec2i _output_sizes;
	public:

		VulkanPostprocessing();
		~VulkanPostprocessing();

		void SetInputTextures(
			Texture* input,
			Texture* depth,
			Texture* normals,
			Texture* positions,
			Texture* ui);

		void FillCommandBuffer();
		void ResizeOutput(const Vec2i& new_size);
		VulkanTexture* GetOutputTexture();
		VulkanSemaphore* GetBeginSemaphore();
		void Execute(VulkanSemaphore* end_semaphore);

		VulkanBloom* GetBloom();
		VulkanSSAO* GetSSAO();

		void Create();
		void Destroy();
	};
}