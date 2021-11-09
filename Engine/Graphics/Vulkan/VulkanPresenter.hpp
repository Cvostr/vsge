#pragma once

#include "VulkanRenderpass.hpp"
#include "VulkanFramebuffer.hpp"
#include <vector>

namespace VSGE {
	class VulkanPresenter {
	private:
		VulkanRenderPass* _renderpass;
		std::vector<VulkanFramebuffer*> _fbs;
		bool _recreated;

		void Create();
		void Destroy();
	public:

		VulkanPresenter();
		~VulkanPresenter();

		VulkanRenderPass* GetRenderPass();
		VulkanFramebuffer* GetFramebuffer(uint32 index);
		void Recreate();

		bool IsRecreated();
		void Update(VkResult imageResult);
	};
}