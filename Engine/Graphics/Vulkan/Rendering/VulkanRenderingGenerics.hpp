#pragma once

#include "../VulkanTexture.hpp"
#include "../VulkanSampler.hpp"
#include "../VulkanMesh.hpp"
#include "PBR/Vulkan_BRDF_LUT.hpp"

namespace VSGE {
	class VulkanRenderingGenerics {
	private:
		bool m_initialized;
		VulkanTexture* m_emptyZeroTexture;
		VulkanTexture* m_emptyZeroCubeTexture;
		VulkanTexture* m_emptyOneTexture;
		VulkanTexture* m_emptyOneCubeTexture;
		VulkanTexture* m_emptyZero2dArrayTexture;

		//--------------------Samplers-------------------
		VulkanSampler* m_attachmentSampler;
		VulkanSampler* m_samplerIBL;
		VulkanSampler* m_materialMapsSampler;

		//--------------------Meshes--------------------
		VulkanMesh* m_screenMesh;

		Vulkan_BRDF_LUT* m_brdfLut;

		static VulkanRenderingGenerics* _this;
	public:

		VulkanRenderingGenerics();

		static VulkanRenderingGenerics* Get();

		bool Init();

		VulkanTexture* GetBlackTexture();

		VulkanTexture* GetWhiteTexture();

		VulkanTexture* GetBlackCubeTexture();

		VulkanTexture* GetWhiteCubeTexture();

		VulkanTexture* GetBlack2dArrayTexture();

		VulkanSampler* GetAttachmentSampler();

		VulkanSampler* GetSpecularIBLSampler();

		VulkanSampler* GetMaterialMapsSampler();

		VulkanMesh* GetScreenMesh();

		Vulkan_BRDF_LUT* GetBRDF();
	};
}