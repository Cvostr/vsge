#pragma once

#include "../Engine/ApplicationLayer.hpp"

namespace VSGE {

	class InputLayer : public IApplicationLayer{
	private:

	public:

		InputLayer();
		~InputLayer();

		void OnAttach();
		void OnUpdate();
		void OnDetach();

		void OnEvent(const VSGE::IEvent& event);
		void OnMouseMotion(const VSGE::EventMouseMotion& motion);
		void OnMouseScroll(const VSGE::EventMouseScrolled& scroll);
		void OnMouseButtonDown(const VSGE::EventMouseButtonDown& mbd);
		void OnMouseButtonUp(const VSGE::EventMouseButtonUp& mbu);
		void OnKeyUp(const VSGE::EventKeyButtonUp& kbd);
		void OnKeyDown(const VSGE::EventKeyButtonDown& kbd);
	};
}