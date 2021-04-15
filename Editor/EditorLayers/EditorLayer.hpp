#pragma once

#include <Engine/ApplicationLayer.hpp>
#include <Scene/Scene.hpp>

namespace VSGEditor {

	class EditorSettings {

	};

	class EditorLayer : public VSGE::IApplicationLayer {
	private:
		static EditorLayer* _this;
		VSGE::Scene* mScene;
	public:

		EditorLayer() {
			mScene = new VSGE::Scene;
			mScene->NewScene();
			_this = this;
		}

		static EditorLayer* Get() {
			return _this;
		}

		VSGE::Scene* GetScene() {
			return mScene;
		}

		void OnAttach();
		void OnUpdate();
		void OnDetach();

	};
}