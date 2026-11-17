#pragma once

#include <vector>
#include <Math/Mat4.hpp>
#include <Math/Rect.hpp>

namespace VSGE {

	class View {
	protected:
		std::vector<View*> _children;
		View* _parent;

		// For rendering
		Rect _global_bounds;

		bool _visible;
		float _rotation;

		// Normalized
		Vec2 anchorMin;
		Vec2 anchorMax;
		Vec2 pivot;

		// Pixels
		Vec2 position;
		Vec2 size;

		bool IsChildExist(View* view);
		void SetParent(View* parent);
		
	public:

		View();
		~View();

		void AddChild(View* view);
		void RemoveChild(View* view);
		std::vector<View*>& GetChildren();

		View* GetParent();
		bool HasParent();
		
		void SetVisible(bool visible);
		bool IsVisible();

		/// <summary>
		/// Sets an anchor of parent in normalized value (0.0 - 1.0)
		/// Anchors determine which part of the parent we attach to
		/// </summary>
		/// <param name="min"></param>
		/// <param name="max"></param>
		void SetAnchors(const Vec2& min, const Vec2& max);
		/// <summary>
		/// Set Pivot of this View
		/// Pivot determines which point of the widget itself we cling to this place 
		/// (and from which point we calculate its position and size).
		/// </summary>
		/// <param name="pivot"></param>
		void SetPivot(const Vec2& pivot);
		/// <summary>
		/// Set position offset in virtual pixels
		/// </summary>
		/// <param name="position"></param>
		void SetPosition(const Vec2& position);
		/// <summary>
		/// Set scale in virtual pixels
		/// </summary>
		/// <param name="position"></param>
		void SetSize(const Vec2& size);

		const float GetRotation();
		void SetRotation(float rotation);

		const Rect& GetGlobalBounds();
		void UpdateGlobalRect(const Rect& parent);

		virtual void Draw(){}
	};
}