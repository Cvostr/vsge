#pragma once
#include <Engine/ApplicationLayer.hpp>
#include "View.hpp"

namespace VSGE {
	class UiLayer : public IApplicationLayer {
	private:
		static UiLayer* _this;
		View* _root;
	public:

		UiLayer();
		~UiLayer();

		static UiLayer* Get() {
			return _this;
		}

		void OnAttach();
		void OnUpdate();
		void OnDetach();

		View* GetRootView();
	};
}