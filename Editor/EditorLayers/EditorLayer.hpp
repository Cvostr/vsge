#pragma once

#include <Engine/ApplicationLayer.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Camera.hpp>
#include <System/FileWatcher.hpp>
#include <Resources/ResourceCache.hpp>

namespace VSGEditor {

	class EditorSettings {

	};

	class EditorLayer : public VSGE::IApplicationLayer {
	private:
		static EditorLayer* _this;
		VSGE::Scene* mScene;
		VSGE::Camera* mEditorCamera;
		VSGE::FileWatcher* mResourcesWatcher;
	public:

		EditorLayer() {
			mScene = new VSGE::Scene;
			mEditorCamera = new VSGE::Camera;
			mScene->NewScene();
			_this = this;
			mResourcesWatcher = new VSGE::FileWatcher;
		}

		~EditorLayer() {
			delete mScene;
			delete mEditorCamera;
			delete mResourcesWatcher;
		}

		static EditorLayer* Get() {
			return _this;
		}

		VSGE::Scene* GetScene() {
			return mScene;
		}

		VSGE::Camera* GetCamera() {
			return mEditorCamera;
		}

		void OpenProjectDirectory(const std::string& dir_path);

		void OnAttach();
		void OnUpdate();
		void OnDetach();

	};
}