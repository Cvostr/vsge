#pragma once

#include "../../../PostprocessEffect.hpp"
#include <Graphics/Vulkan/VulkanTexture.hpp>
#include <Graphics/Vulkan/VulkanPipeline.hpp>
#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include <Graphics/Vulkan/VulkanShader.hpp>
#include <Graphics/Vulkan/VulkanRenderpass.hpp>

namespace VSGE {
	class VulkanBloomBase {
	private:
		VulkanShader* _bloom_bright_shader;
		VulkanShader* _bloom_blur_v_shader;
		VulkanShader* _bloom_blur_h_shader;

		VulkanPipelineLayout* _bloom_bright_playout;

		VulkanPipeline* _bloom_bright_pipeline;
		VulkanPipeline* _bloom_blur_v_pipeline;
		VulkanPipeline* _bloom_blur_h_pipeline;
	public:
		VulkanBloomBase();
		~VulkanBloomBase();
		void Create();
		void Destroy();

		VulkanRenderPass* CreateBrightRenderpass();

		VulkanPipeline* GetBrightnessPipeline();
		VulkanPipeline* GetBlurVPipeline();
		VulkanPipeline* GetBlurHPipeline();
		VulkanPipelineLayout* GetBrightnessPipelineLayout();
	};

	class VulkanBloom : public PostprocessEffect {
	private:
		VulkanDescriptorPool* _descr_pool;
		VulkanDescriptorSet* _bright_descr_set;

		VulkanDescriptorSet* _blur_1_descr_set;
		VulkanDescriptorSet* _blur_2_descr_set;

		VulkanTexture* _input_texture;

		VulkanRenderPass* _rpass;
		VulkanFramebuffer* _fb_bright;
		VulkanFramebuffer* _fb_1;
		VulkanFramebuffer* _fb_2;
		
	public:
		VulkanBloom();
		~VulkanBloom();

		void Create();
		void Destroy();

		void SetInputTextureHDR(VulkanTexture* input);
		VulkanTexture* GetBlurredBloomTextureHDR();
		void RecordCommandBuffer(VulkanCommandBuffer* cmdbuf);
		void ResizeOutput(const Vec2i& new_size);
	};
}