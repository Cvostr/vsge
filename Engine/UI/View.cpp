#include "View.hpp"

using namespace VSGE;

View::View() {

}
View::~View() {

}

void View::AddChild(View* view) {

}
void View::RemoveChild(View* view) {

}
std::vector<View*>& View::GetChildren() {
	return _children;
}