#pragma once

#include "../Engine/ApplicationLayer.hpp"

namespace VSGE {
	class MonoLayer : public IApplicationLayer {
	public:
		void OnAttach();
		void OnUpdate();
		void OnDetach();
	};
}