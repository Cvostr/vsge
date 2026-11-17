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

		Vec2i _referenceResolution;
		float _resolutionScale;

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

		/// <summary>
		/// Sets a reference UI resolution, around which all UI elements are composed,
		/// It is not an actual screen or UI render resolution. It is UI design resolution
		/// Default it FullHD (1920x1080)
		/// </summary>
		void SetReferenceResolution(const Vec2i& resolution);
		/// <summary>
		/// Obtain a root UI element for placing other elements
		/// </summary>
		/// <returns></returns>
		View* GetRootView();
		/// <summary>
		/// Gets virtual pixel aware cursor position in float values
		/// This method should be used for UI checks
		/// </summary>
		/// <returns></returns>
		Vec2 GetMouseCursorPos();
	};
}