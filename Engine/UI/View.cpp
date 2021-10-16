#include "View.hpp"
#include <algorithm>
#include <Math/MatrixTransform.hpp>

using namespace VSGE;

View::View() :
	_parent(nullptr),
	_visible(true)
	
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
const Rect& View::GetBounds() {
	return _bounds;
}
void View::SetBounds(const Rect& bounds) {
	_bounds = bounds;
}
void View::SetMargin(float left, float right, float top, float bottom) {
	_transform.margin.left = left;
	_transform.margin.right = right;
	_transform.margin.top = top;
	_transform.margin.bottom = bottom;
}
const Margin& View::GetMargin() {
	return _transform.margin;
}

const Vec2& View::GetAnchorMin() {
	return _transform.anchorMin;
}
const Vec2& View::GetAnchorMax() {
	return _transform.anchorMax;
}
void View::SetAnchorMin(const Vec2& anchor) {
	_transform.anchorMin = anchor;
}
void View::SetAnchorMax(const Vec2& anchor) {
	_transform.anchorMax = anchor;
}

const Vec2& View::GetScale() {
	return _transform.scale;
}
void View::SetScale(const Vec2& scale) {
	_transform.scale = scale;
}

const float View::GetRotation() {
	return _transform.rotation;
}
void View::SetRotation(float rotation) {
	_transform.rotation = rotation;
}

void View::UpdateBounds() {
	Margin anchors;
	Vec2 offset;
	if (HasParent()) {
		Rect parentBounds = _parent->GetBounds();

		anchors = Margin
		(
			_transform.anchorMin.x * parentBounds.Size.x,
			_transform.anchorMax.x * parentBounds.Size.x,
			_transform.anchorMin.y * parentBounds.Size.y,
			_transform.anchorMax.y * parentBounds.Size.y
		);
		offset = parentBounds.Pos;
	}
	else{
		anchors = Margin();
		offset = Vec2(0, 0);
	}

	_bounds.Pos.x = anchors.left + _transform.margin.left;
	if (_transform.anchorMin.x != _transform.anchorMax.x){
		_bounds.Size.x = anchors.right - _bounds.Pos.x - _transform.margin.right;
	}
	else{
		_bounds.Size.x = _transform.margin.right;
	}

	_bounds.Pos.y = anchors.top + _transform.margin.top;
	if (_transform.anchorMin.y != _transform.anchorMax.y){
		_bounds.Size.y = anchors.bottom - _bounds.Pos.y - _transform.margin.bottom;
	}
	else{
		_bounds.Size.y = _transform.margin.bottom;
	}

	_bounds.Pos += offset;
}

Mat4 View::GetViewTransform() {
	UpdateBounds();

	Mat4 translation = GetTranslationMatrix(Vec3(_bounds.Pos, 0));
	Mat4 scale = GetScaleMatrix(Vec3(_bounds.Size, 1));

	Vec2 rotation_pos = _bounds.Size * _transform.pivot;
	Mat4 rotation = GetRotationZMatrixEuler(_transform.rotation);
	Mat4 pre_rotation = GetTranslationMatrix(Vec3(rotation_pos * -1, 0));
	Mat4 post_rotation = GetTranslationMatrix(Vec3(rotation_pos, 0));
	rotation = pre_rotation * rotation * post_rotation;

	Mat4 result = scale * rotation * translation;
	return result;
}