#include "View.hpp"
#include <algorithm>
#include <Math/MatrixTransform.hpp>

using namespace VSGE;

View::View() :
	_parent(nullptr),
	_visible(true),
	_anchorMin(0.f),
	_anchorMax(1.f),
	_scale(1.f),
	_pivot(0.5f),
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

	view->SetParent(view);
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
const Rect& View::GetBounds() {
	return _bounds;
}
void View::SetMargin(float left, float right, float top, float bottom) {
	_margin.Left = left;
	_margin.Right = right;
	_margin.Top = top;
	_margin.Bottom = bottom;
}
const Margin& View::GetMargin() {
	return _margin;
}

const Vec2& View::GetAnchorMin() {
	return _anchorMin;
}
const Vec2& View::GetAnchorMax() {
	return _anchorMax;
}
void View::SetAnchorMin(const Vec2& anchor) {
	_anchorMin = anchor;
}
void View::SetAnchorMax(const Vec2& anchor) {
	_anchorMax = anchor;
}

const Vec2& View::GetScale() {
	return _scale;
}
void View::SetScale(const Vec2& scale) {
	_scale = scale;
}

const float View::GetRotation() {
	return _rotation;
}
void View::SetRotation(float rotation) {
	_rotation = rotation;
}

void View::UpdateBounds() {
	Margin anchors;
	Vec2 offset;
	if (HasParent()) {
		Rect parentBounds;

		anchors = Margin
		(
			_anchorMin.x * parentBounds.Size.x,
			_anchorMax.x * parentBounds.Size.x,
			_anchorMin.y * parentBounds.Size.y,
			_anchorMax.y * parentBounds.Size.y
		);
		offset = parentBounds.Pos;
	}
	else{
		anchors = Margin();
		offset = Vec2(0, 0);
	}

	_bounds.Pos.x = anchors.Left + _margin.Left;
	if (_anchorMin.x != _anchorMax.x){
		_bounds.Size.x = anchors.Right - _bounds.Pos.x - _margin.Right;
	}
	else{
		_bounds.Size.x = _margin.Right;
	}

	_bounds.Pos.y = anchors.Top + _margin.Top;
	if (_anchorMin.y != _anchorMax.y){
		_bounds.Size.y = anchors.Bottom - _bounds.Pos.y - _margin.Bottom;
	}
	else{
		_bounds.Size.y = _margin.Bottom;
	}

	_bounds.Pos += offset;
}

Mat4 View::GetViewTransform() {
	Vec2 v1 = Vec2(_pivot.x * _bounds.Size.x, _pivot.y * _bounds.Size.y);
	Vec2 v2 = v1 * -1;
	v1 += _bounds.Pos;

	Mat4 scale = GetScaleMatrix(Vec3(_scale, 1.f));
	
	Mat4 translation_v2 = GetTranslationMatrix(Vec3(v2, 0));
	Mat4 trans_v2_scale = translation_v2 * scale;
	Mat4 translation_v1 = GetTranslationMatrix(Vec3(v1, 0));
	Mat4 result = trans_v2_scale * translation_v1;

	return result;
}