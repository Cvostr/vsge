#pragma once

#include <vector>
#include <Math/Mat4.hpp>
#include <Math/Rect.hpp>

namespace VSGE {

	class View {
	protected:
		std::vector<View*> _children;
		View* _parent;

		bool _visible;
		Rect _bounds;
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
		const Rect& GetBounds();
		void SetBounds(const Rect& bounds);

		const float GetRotation();
		void SetRotation(float rotation);

		virtual void Draw(){}
	};
}