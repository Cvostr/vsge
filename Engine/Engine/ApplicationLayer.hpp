#pragma once

#include <vector>

namespace VSGE {
	class IApplicationLayer {
	public:
		virtual void OnAttach() = 0;
		virtual void OnDetach() {}
		virtual void OnUpdate() = 0;

		virtual void OnSDL2Event(void* event_ptr) {}
	};

	typedef std::vector<IApplicationLayer*> tApplicationLayerList;
}