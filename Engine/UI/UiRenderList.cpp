#include "UiRenderList.hpp"
#include <Graphics/FontManager.hpp>
#include <Core/Logger.hpp>

using namespace VSGE;

std::vector<UiRenderTask>& UiRenderList::GetTasks() {
	return _tasks;
}
void UiRenderList::ClearTasks() {
	_tasks.clear();
}

void UiRenderList::DrawSprite(const Rect& bounds, 
							float rotation, 
							TextureResource* sprite, 
							const Vec2& uv_min,
							const Vec2& uv_max)
{
	UiRenderTask task;
	task._type = UI_RENDER_TASK_TYPE_SPRITE;
	task.rotation = rotation;
	task.bounds = bounds;
	task._sprite = sprite;
	task.uv_min = uv_min;
	task.uv_max = uv_max;
	_tasks.push_back(task);
}
void UiRenderList::DrawText(const Rect& bounds, float rotation, const String& text, const std::string& fontName, const Color& color) 
{
	GlyphManager* glyph_manager = GlyphManager::Get();
	GlyphFontContainer* font = glyph_manager->GetFontByName(fontName);
	if (font == nullptr) {
		Logger::Log(LogType::LOG_TYPE_ERROR) << "No font '" << fontName << "'" << "\n";
		return;
	}
	
	UiRenderTask task;
	task._type = UI_RENDER_TASK_TYPE_TEXT;
	task.rotation = rotation;
	task.bounds = bounds;
	task._text = text;
	task.font = font;
	task._color = color;
	_tasks.push_back(task);
}