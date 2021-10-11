#include "Rect.hpp"

Vec2 Rect::GetCenter() const {
	return Pos + Size * 0.5f;
}
float Rect::GetTop() const {
	return Pos.y;
}
float Rect::GetBottom() const {
	return Pos.y + Size.y;
}
float Rect::GetLeft() const {
	return Pos.x;
}
float Rect::GetRight() const {
	return Pos.x + Size.x;
}