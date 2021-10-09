#pragma once

#include <vector>
#include <Math/Vec2.hpp>

namespace VSGE {

	class Margin {
	public:
		float Left;
		float Right;
		float Top;
		float Bottom;

		Margin() {}
		Margin(float left, float right, float top, float bottom) : 
			Left(left), Right(right), Top(top), Bottom(bottom) {}
	};

	class View {
	private:
		std::vector<View*> _children;
		View* _parent;

		bool _visible;
		Margin _margin;
		Vec2 _scale;
		float _rotation;

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
		void SetMargin(float left, float right, float top, float bottom);
		const Margin& GetMargin();
		
		const Vec2& GetScale();
	};
}