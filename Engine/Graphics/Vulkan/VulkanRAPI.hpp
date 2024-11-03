#pragma once

#include "../RenderingAPI.hpp"

#include "VulkanInstance.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanMA.hpp"

namespace VSGE {
	class VulkanRAPI : public IRenderingAPI {
	private:

		static VulkanRAPI* _this;

		VulkanInstance* Instance;
		VulkanDevice* Device;
		VulkanSwapChain* SwapChain;
		VulkanMA* Allocator;
	public:

		VulkanRAPI();

		void Create(Window* window, bool debugGraphics = false);

		void Destroy();

		VulkanInstance* GetInstance();
		VulkanDevice* GetDevice();
		VulkanMA* GetAllocator();
		VulkanSwapChain* GetSwapChain();

		static VulkanRAPI* Get() {
			return _this;
		}
	};
}