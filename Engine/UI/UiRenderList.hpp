#pragma once

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
		Rect bounds;
		float rotation;
		Vec2 uv_min;
		Vec2 uv_max;

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

		void DrawSprite(const Rect& bounds, 
						float rotation, 
						TextureResource* sprite,
						const Vec2& uv_min = Vec2(0,0),
						const Vec2& uv_max = Vec2(1, 1));
		void DrawText(const Rect& bounds, float rotation, const String& text, const std::string& font, const Color& text_color);
	};
}