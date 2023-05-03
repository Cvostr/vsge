#pragma once

#include <Graphics/Vulkan/VulkanBuffer.hpp>
#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include <Graphics/Vulkan/VulkanFramebuffer.hpp>
#include <Graphics/Vulkan/VulkanRenderpass.hpp>
#include <Graphics/Vulkan/VulkanShader.hpp>
#include <Graphics/Vulkan/VulkanPipeline.hpp>
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>
#include <Graphics/Vulkan/VulkanSynchronization.hpp>

#define MAX_UI_ELEMENTS 500
#define MAX_UI_TEXT_ELEMENTS 300

namespace VSGE {
	class VulkanUiRenderer {
	private:
		VulkanBuffer* _transforms_buffer;
		VulkanBuffer* _frag_buffer;
		VulkanDescriptorPool* _descr_pool;
		std::vector<VulkanDescriptorSet*> _descr_sets;
		VulkanSampler* _ui_sampler;

		VulkanFramebuffer* _ui_framebuffer;
		VulkanRenderPass* _ui_rp;

		VulkanPipelineLayout* _ui_pll;
		VulkanPipeline* _ui_pipeline;

		VulkanMesh* _ui_sprite_mesh;

		uint32 written_elements;
		Mat4 _camera_transform;
		uint32 _fb_width;
		uint32 _fb_height;

		void WriteTransform(uint32 elem_id, const Mat4& transform);
		void WriteElement(uint32 elem_id, const Vec2& uv_min, const Vec2& uv_max, const Color& color, int text = 0);
		void WriteTexture(uint32 elem_id, VulkanTexture* texture);
	public:

		VulkanUiRenderer();
		~VulkanUiRenderer();

		void Create();
		void Destroy();
		void ResizeOutput(uint32 width, uint32 height);
		VulkanTexture* GetOutputTexture();

		void FillBuffers();
		void RecordCommandBuffer(VulkanCommandBuffer* cmdbuffer);
	};
}