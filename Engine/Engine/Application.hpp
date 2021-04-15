#pragma once

#include "ApplicationLayer.hpp"
#include "Window.hpp"

namespace VSGE {
	class Application {
	private:

		tApplicationLayerList mLayers;

		Window* mWindow;

		bool mRunning;
	public:

		static Application* _this;

		Application() :
						mRunning(false),
						mWindow(new Window())
		{
			_this = this;
		}

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

		Window& GetWindow() { return *mWindow; }
		/// <summary>
		/// Calls OnUpdate on all registered application layers
		/// </summary>
		void OnUpdate();

		static Application* Get() {
			return _this;
		}
	};
}