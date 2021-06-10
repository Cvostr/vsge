#pragma once

#include "../Engine/ApplicationLayer.hpp"

namespace VSGE {
	class AudioLayer : public IApplicationLayer {
	public:
		void OnAttach();
		void OnUpdate();
		void OnDetach();
	};
}