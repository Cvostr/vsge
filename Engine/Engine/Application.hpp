#pragma once

#include "ApplicationLayer.hpp"
#include "Window.hpp"
#include <string>
#include <Core/Threading/Mutex.hpp>

namespace VSGE {

	class IRenderer;

	enum GraphicsApi {
		GRAPHICS_API_VULKAN,
		GRAPHICS_API_D3D12
	};

	struct ApplicationCreateInfo {
		std::string ApplicationName;
		uint32 ApplicationVersion;

		std::string application_dir;
		bool server;
		bool headless;
		GraphicsApi graphicsApi;

		ApplicationCreateInfo(std::string name, uint32 version) {
			ApplicationName = name;
			ApplicationVersion = version;
			headless = false;
			server = false;
			graphicsApi = GRAPHICS_API_VULKAN;
		}
	};

	class Application {
	private:

		ApplicationCreateInfo _description;
		//registered application layers list
		tApplicationLayerList _layers;
		//Events, created (usually) from other thread
		std::vector<IEvent*> _queuedEvents;
		Mutex* _queuedEventsMutex;

		Window* _window;
		IRenderer* _renderer;

		bool _running;
	public:

		static Application* _this;

		Application(ApplicationCreateInfo descr);
		~Application();

		const ApplicationCreateInfo& GetDescription() { return _description; }

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
		Window& GetWindow() { return *_window; }
		/// <summary>
		/// Calls OnUpdate on all registered application layers
		/// </summary>
		void OnUpdate();

		void SetRenderer(IRenderer* renderer);
		IRenderer* GetRenderer();

		void OnSDL2Event(void* event);
		void OnEvent(const IEvent& event);

		void QueueEvent(const IEvent* event);

		template<typename T>
		T* GetLayer() {
			for (auto layer : _layers) {
				if (typeid(*layer) == typeid(T)) {
					return static_cast<T*>(layer);
				}
			}
			return nullptr;
		}

		GraphicsApi GetGraphicsApi() {
			return _description.graphicsApi;
		}

		static Application* Get() {
			return _this;
		}
	};
}