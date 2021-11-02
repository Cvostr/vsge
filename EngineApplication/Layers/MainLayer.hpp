#pragma once

#include <Engine/ApplicationLayer.hpp>

namespace VSGE {
	class MainLayer : public IApplicationLayer {
	public:
		void OnAttach();
		void OnUpdate();
		void OnDetach();
	};
}