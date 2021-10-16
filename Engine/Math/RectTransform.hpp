#pragma once

#include "Rect.hpp"
#include "Vec2.hpp"

class Margin {
public:
	float left;
	float right;
	float top;
	float bottom;

	Margin() :
		left(0), right(0), top(0), bottom(0) {}

	Margin(float left, float right, float top, float bottom) :
		left(left), right(right), top(top), bottom(bottom) {}
};

class RectTransform {

public:
	Margin margin;
	Vec2 anchorMin;
	Vec2 anchorMax;
	Vec2 pivot;
	Vec2 scale;
	float rotation;

	RectTransform() : 
		anchorMin(0.f),
		anchorMax(1.f),
		scale(1.f),
		pivot(0.5f),
		rotation(0.f) {}
};