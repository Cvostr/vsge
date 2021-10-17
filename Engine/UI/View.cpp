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
const Rect& View::GetBounds() {
	return _bounds;
}
void View::SetBounds(const Rect& bounds) {
	_bounds = bounds;
}

const float View::GetRotation() {
	return _rotation;
}
void View::SetRotation(float rotation) {
	_rotation = rotation;
}