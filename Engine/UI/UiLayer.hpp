#pragma once
#include <Engine/ApplicationLayer.hpp>
#include "View.hpp"
#include "UiRenderList.hpp"

namespace VSGE {
	class UiLayer : public IApplicationLayer {
	private:
		static UiLayer* _this;
		View* _root;
		UiRenderList* _render_list;
	public:

		UiLayer();
		~UiLayer();

		static UiLayer* Get() {
			return _this;
		}

		UiRenderList* GetRenderList();

		void OnAttach();
		void OnUpdate();
		void OnDetach();

		View* GetRootView();
	};
}