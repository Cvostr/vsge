#pragma once

#include <Math/RectTransform.hpp>
#include <Math/Rect.hpp>
#include <Math/Color.hpp>
#include <vector>
#include <Core/VarTypes/String.hpp>
#include <Resources/ResourceTypes/TextureResource.hpp>

namespace VSGE {

	enum UiRenderTaskType {
		UI_RENDER_TASK_TYPE_SPRITE,
		UI_RENDER_TASK_TYPE_TEXT
	};

	class UiRenderTask {
	public:
		UiRenderTaskType _type;
		RectTransform transform;
		Rect bounds;

		TextureResource* _sprite;
		String _text;
		Color _color;
		std::string font;
	};

	class UiRenderList {
	private:
		std::vector<UiRenderTask> _tasks;
	public:
		std::vector<UiRenderTask>& GetTasks();
		void ClearTasks();

		void DrawSprite(const RectTransform& transform, const Rect& bounds, TextureResource* sprite);
		void DrawText(const RectTransform& transform, const Rect& bounds, const String& text, const std::string& font, const Color& text_color);
	};
}