#include "View.hpp"
#include <algorithm>
#include <Math/MatrixTransform.hpp>

using namespace VSGE;

View::View() :
	_parent(nullptr),
	_visible(true),
	_rotation(0.f)
{
}
View::~View() {

}

bool View::IsChildExist(View* view) {
	for (auto& child : _children) {
		if (child == view)
			return true;
	}
	return false;
}
void View::AddChild(View* view) {
	if (IsChildExist(view))
		return;
	
	if (view->HasParent()) {
		view->GetParent()->RemoveChild(view);
	}

	view->SetParent(this);
	_children.push_back(view);
}
void View::RemoveChild(View* view) {
	if (IsChildExist(view))
		return;

	view->SetParent(nullptr);

	std::remove(_children.begin(), _children.end(), view);
	_children.pop_back();
}
std::vector<View*>& View::GetChildren() {
	return _children;
}
void View::SetParent(View* parent) {
	_parent = parent;
}
View* View::GetParent() {
	return _parent;
}
bool View::HasParent() {
	return _parent != nullptr;
}

void View::SetVisible(bool visible) {
	_visible = visible;
}
bool View::IsVisible() {
	return _visible;
}

void View::SetAnchors(const Vec2& min, const Vec2& max) {
	anchorMin = min;
	anchorMax = max;
}
void View::SetPivot(const Vec2& pivot) {
	this->pivot = pivot;
}
void View::SetPosition(const Vec2& position) {
	this->position = position;
}
void View::SetSize(const Vec2& size) {
	this->size = size;
}

const Rect& View::GetGlobalBounds() {
	return _global_bounds;
}

void View::UpdateGlobalRect(const Rect& parent) 
{
	Vec2 anchorMinPos = parent.Pos + parent.Size * anchorMin;
	Vec2 anchorMaxPos = parent.Pos + parent.Size * anchorMax;
	Vec2 anchorBoxSize = anchorMaxPos - anchorMinPos;

	// Absolute size of element
	_global_bounds.Size = anchorBoxSize + size;

	// Pivot
	Vec2 anchorPivotPos = anchorMinPos + anchorBoxSize * pivot;
	Vec2 widgetPivotPos = anchorPivotPos + position;

	// Pos - Upper Left corner
	_global_bounds.Pos = widgetPivotPos - _global_bounds.Size * pivot;

	// Update children
	for (auto child : _children) {
		if (child->IsVisible()) {
			child->UpdateGlobalRect(_global_bounds);
		}
	}
}

const float View::GetRotation() {
	return _rotation;
}
void View::SetRotation(float rotation) {
	_rotation = rotation;
}