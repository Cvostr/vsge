#pragma once

#include <Math/Vec2.hpp>


	class Rect {
	public:
		Vec2 Pos;
		Vec2 Size;

		Rect(){}

		Rect(float x, float y, float width, float height): Pos(x,y), Size(width, height)
		{}

		Rect(const Vec2& pos, const Vec2& size) : Pos(pos), Size(size) {}

		Vec2 GetCenter() const;

		float GetTop() const;
		float GetBottom() const;
		float GetLeft() const;
		float GetRight() const;
	};
