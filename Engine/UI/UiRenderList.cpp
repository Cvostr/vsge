#include "UiRenderList.hpp"

using namespace VSGE;

std::vector<UiRenderTask>& UiRenderList::GetTasks() {
	return _tasks;
}
void UiRenderList::ClearTasks() {
	_tasks.clear();
}

void UiRenderList::DrawSprite(const Mat4& transform, TextureResource* sprite) {
	UiRenderTask task;
	task._type = UI_RENDER_TASK_TYPE_SPRITE;
	task._transform = transform;
	task._sprite = sprite;
	_tasks.push_back(task);
}
void UiRenderList::DrawText(const Mat4& transform, const String& text) {
	UiRenderTask task;
	task._type = UI_RENDER_TASK_TYPE_TEXT;
	task._transform = transform;
	task._text = text;
	_tasks.push_back(task);
}