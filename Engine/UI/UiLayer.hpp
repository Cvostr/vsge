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

		void DrawAll();
		void DrawView(View* view);
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
		/// <summary>
		/// Gets virtual pixel aware cursor position in float values
		/// This method should be used for UI checks
		/// </summary>
		/// <returns></returns>
		Vec2 GetMouseCursorPos();
	};
}