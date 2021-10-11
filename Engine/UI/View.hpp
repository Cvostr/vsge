#pragma once

#include <vector>
#include <Math/Rect.hpp>
#include <Math/Vec2.hpp>
#include <Math/Mat4.hpp>

namespace VSGE {

	class Margin {
	public:
		float Left;
		float Right;
		float Top;
		float Bottom;

		Margin() :
			Left(0), Right(0), Top(0), Bottom(0) {}

		Margin(float left, float right, float top, float bottom) : 
			Left(left), Right(right), Top(top), Bottom(bottom) {}
	};

	class View {
	private:
		std::vector<View*> _children;
		View* _parent;

		bool _visible;
		Rect _bounds;
		Margin _margin;
		Vec2 _anchorMin;
		Vec2 _anchorMax;
		Vec2 _scale;
		Vec2 _pivot;
		float _rotation;

		bool IsChildExist(View* view);
		void UpdateBounds();
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

		Mat4 GetViewTransform();

		virtual void Draw(){}
	};
}