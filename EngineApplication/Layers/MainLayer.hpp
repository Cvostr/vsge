#pragma once

#include <Engine/ApplicationLayer.hpp>
#include <Scene/SceneLayer.hpp>

namespace VSGE {
	class MainLayer : public IApplicationLayer {
	private:
		
	public:
		MainLayer();

		void OnAttach();
		void OnUpdate();
		void OnDetach();

		void OnEvent(const VSGE::IEvent& event);
		void OnSceneLoadedEvent(const VSGE::SceneLoadedEvent& sle);
	};
}