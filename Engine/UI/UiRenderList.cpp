#include "UiRenderList.hpp"

using namespace VSGE;

std::vector<UiRenderTask>& UiRenderList::GetTasks() {
	return _tasks;
}
void UiRenderList::ClearTasks() {
	_tasks.clear();
}

void UiRenderList::DrawSprite(const Rect& bounds, float rotation, TextureResource* sprite) {
	UiRenderTask task;
	task._type = UI_RENDER_TASK_TYPE_SPRITE;
	task.rotation = rotation;
	task.bounds = bounds;
	task._sprite = sprite;
	_tasks.push_back(task);
}
void UiRenderList::DrawText(const Rect& bounds, float rotation, const String& text, const std::string& font, const Color& color) {
	UiRenderTask task;
	task._type = UI_RENDER_TASK_TYPE_TEXT;
	task.rotation = rotation;
	task.bounds = bounds;
	task._text = text;
	task.font = font;
	task._color = color;
	_tasks.push_back(task);
}