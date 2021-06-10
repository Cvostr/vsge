#pragma once

#include "../Engine/ApplicationLayer.hpp"

namespace VSGE {
	class PhysicsLayer : public IApplicationLayer {
	public:
		void OnAttach();
		void OnUpdate();
		void OnDetach();
	};
}