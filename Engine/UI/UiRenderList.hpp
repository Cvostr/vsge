#pragma once

#include <Math/Mat4.hpp>
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
		Mat4 _transform;

		TextureResource* _sprite;
		String _text;
	};

	class UiRenderList {
	private:
		std::vector<UiRenderTask> _tasks;
	public:
		std::vector<UiRenderTask>& GetTasks();
		void ClearTasks();

		void DrawSprite(const Mat4& transform, TextureResource* sprite);
		void DrawText(const Mat4& transform, const String& text);
	};
}