#pragma once

#include "../VulkanTexture.hpp"
#include "../VulkanSampler.hpp"

namespace VSGE {
	class VulkanRenderingGenerics {
	private:
		bool m_initialized;
		VulkanTexture* m_emptyZeroTexture;
		VulkanTexture* m_emptyZeroCubeTexture;
		VulkanTexture* m_emptyOneTexture;
		VulkanTexture* m_emptyOneCubeTexture;
		VulkanTexture* m_emptyZero2dArrayTexture;

		VulkanSampler* m_samplerIBL;
		VulkanSampler* m_materialMapsSampler;

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

		VulkanSampler* GetSpecularIBLSampler();

		VulkanSampler* GetMaterialMapsSampler();
	};
}