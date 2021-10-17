#pragma once

#include "VulkanGammaCorrection.hpp"
#include "VulkanBloom.hpp"
#include "../../VulkanCommandBuffer.hpp"

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
		PostprocessSettings _settings;
		VulkanTexture* _output_texture;

		VulkanGammaCorrection* _gamma_correction;

		VulkanCommandPool* _cmdpool;
		VulkanCommandBuffer* _cmdbuf;

		Vec2i _output_sizes;
	public:

		VulkanPostprocessing();
		~VulkanPostprocessing();

		void SetInputTextures(
			Texture* input,
			Texture* depth,
			Texture* normals,
			Texture* positions);

		void FillCommandBuffer();
		void ResizeOutput(const Vec2i& new_size);

		void Create();
		void Destroy();
	};
}