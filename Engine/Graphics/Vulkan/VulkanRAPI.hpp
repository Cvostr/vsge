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

		void Create(Window* window);

		void Destroy();

		VulkanInstance* GetInstance() {
			return Instance;
		}

		VulkanDevice* GetDevice() {
			return Device;
		}

		VulkanMA* GetAllocator() {
			return Allocator;
		}

		VulkanSwapChain* GetSwapChain() {
			return SwapChain;
		}

		VulkanRAPI(){
			_this = this;

			Instance = nullptr;
			Device = nullptr;
			SwapChain = nullptr;
			Allocator = nullptr;
		}

		static VulkanRAPI* Get() {
			return _this;
		}
	};
}