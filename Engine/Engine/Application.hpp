#pragma once

#include "ApplicationLayer.hpp"
#include "Window.hpp"
#include <string>

namespace VSGE {

	enum GraphicsApi {
		GRAPHICS_API_VULKAN,
		GRAPHICS_API_D3D12
	};

	struct ApplicationCreateInfo {
		std::string ApplicationName;
		uint32 ApplicationVersion;

		bool headless;
		GraphicsApi graphicsApi;

		ApplicationCreateInfo(std::string name, uint32 version) {
			ApplicationName = name;
			ApplicationVersion = version;
			headless = false;
			graphicsApi = GRAPHICS_API_VULKAN;
		}
	};

	class Application {
	private:

		ApplicationCreateInfo description;

		tApplicationLayerList mLayers;

		Window* mWindow;

		bool mRunning;
	public:

		static Application* _this;

		Application(ApplicationCreateInfo descr) :
						mRunning(false),
						mWindow(new Window()),
						description(descr)
		{
			_this = this;
		}

		const ApplicationCreateInfo& GetDescription() { return description; }

		void Run();
		void Stop();

		/// <summary>
		/// Register new application layer
		/// </summary>
		/// <param name="layer">Pointer to layer instance</param>
		void AddLayer(IApplicationLayer* layer);
		/// <summary>
		/// Unregister registered application layer
		/// </summary>
		/// <param name="layer">Pointer to layer object</param>
		void RemoveLayer(IApplicationLayer* layer);
		/// <summary>
		/// Removes all registered application layers
		/// </summary>
		void RemoveAllLayers();
		/// <summary>
		/// Get object of window
		/// </summary>
		/// <returns></returns>
		Window& GetWindow() { return *mWindow; }
		/// <summary>
		/// Calls OnUpdate on all registered application layers
		/// </summary>
		void OnUpdate();

		void OnSDL2Event(void* event);
		void OnEvent(IWindowEvent& event);

		template<typename T>
		T* GetLayer() {
			for (auto layer : mLayers) {
				if (typeid(*layer) == typeid(T)) {
					return static_cast<T*>(layer);
				}
			}
			return nullptr;
		}

		GraphicsApi GetGraphicsApi() {
			return description.graphicsApi;
		}

		static Application* Get() {
			return _this;
		}
	};
}