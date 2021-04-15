#pragma once

#include "../RenderingAPI.hpp"

#include "VulkanInstance.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"

namespace VSGE {
	class VulkanRAPI : public IRenderingAPI {
	private:

		static VulkanRAPI* _this;

		Engine::ZSVulkanInstance* Instance;
		Engine::ZSVulkanDevice* Device;
		Engine::ZSVulkanSwapChain* SwapChain;
	public:

		void Create(Window* window);

		void Destroy(){}

		Engine::ZSVulkanInstance* GetInstance() {
			return Instance;
		}

		Engine::ZSVulkanDevice* GetDevice() {
			return Device;
		}

		VulkanRAPI(){
			_this = this;
		}

		static VulkanRAPI* Get() {
			return _this;
		}
	};
}