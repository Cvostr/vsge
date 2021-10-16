#pragma once

#include <vector>
#include <Math/RectTransform.hpp>
#include <Math/Mat4.hpp>

namespace VSGE {

	class View {
	protected:
		std::vector<View*> _children;
		View* _parent;

		bool _visible;
		Rect _bounds;
		RectTransform _transform;

		bool IsChildExist(View* view);
		
	public:

		View();
		~View();

		void AddChild(View* view);
		void RemoveChild(View* view);
		std::vector<View*>& GetChildren();
		void SetParent(View* parent);
		View* GetParent();
		bool HasParent();
		void SetVisible(bool visible);
		bool IsVisible();
		const Rect& GetBounds();
		//DO NOT USE MANUAL
		void SetBounds(const Rect& bounds);

		void SetMargin(float left, float right, float top, float bottom);
		const Margin& GetMargin();

		const Vec2& GetAnchorMin();
		const Vec2& GetAnchorMax();
		void SetAnchorMin(const Vec2& anchor);
		void SetAnchorMax(const Vec2& anchor);
		
		const Vec2& GetScale();
		void SetScale(const Vec2& scale);

		const float GetRotation();
		void SetRotation(float rotation);

		void UpdateBounds();
		Mat4 GetViewTransform();

		virtual void Draw(){}
	};
}