#pragma once

#include <Engine/ApplicationLayer.hpp>

namespace VSGETest {
	class D3D12RectTestLayer : public VSGE::IApplicationLayer {
	private:
		
	public:

		D3D12RectTestLayer() {

		}

		void OnAttach();
		void OnUpdate();
		void OnDetach();
		void OnWindowEvent(VSGE::IWindowEvent& event);
	};
}