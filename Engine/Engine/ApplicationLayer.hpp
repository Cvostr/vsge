#pragma once

#include <vector>
#include "WindowEvent.hpp"

namespace VSGE {
	class IApplicationLayer {
	public:
		virtual void OnAttach() = 0;
		virtual void OnDetach() {}
		virtual void OnUpdate() = 0;

		virtual void OnSDL2Event(void* event_ptr) {}
		virtual void OnWindowEvent(IWindowEvent& event) {}
	};

	typedef std::vector<IApplicationLayer*> tApplicationLayerList;
}