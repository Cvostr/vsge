#include "EditorIcons.hpp"

using namespace VSGEditor;

EditorIcons* EditorIcons::_this = new EditorIcons;

EditorIcons::EditorIcons() {

}
EditorIcons::~EditorIcons() {
    FreeIcons();
}

void EditorIcons::LoadIcons() {
    _sampler.Create();

    _backBtnIcon.CreateFromFile("res/icons/dir_back.png", _sampler);
    _dirIcon.CreateFromFile("res/icons/dir.png", _sampler);
    _unknownFile.CreateFromFile("res/icons/unknown.png", _sampler);
    _3DModelIcon.CreateFromFile("res/icons/3dmodel.png", _sampler);
    _sceneIcon.CreateFromFile("res/icons/3d_scene.png", _sampler);
    _scriptIcon.CreateFromFile("res/icons/script.png", _sampler);
    _audioIcon.CreateFromFile("res/icons/audio-clip.png", _sampler);
    _playIcon.CreateFromFile("res/icons/button_play.png", _sampler);
    _pauseIcon.CreateFromFile("res/icons/button_pause.png", _sampler);
    _stopIcon.CreateFromFile("res/icons/button_stop.png", _sampler);
    _translateIcon.CreateFromFile("res/icons/translate.png", _sampler);
    _scaleIcon.CreateFromFile("res/icons/scale.png", _sampler);
    _rotateIcon.CreateFromFile("res/icons/rotate.png", _sampler);
}

void EditorIcons::FreeIcons() {
    _sampler.Destroy();
}

void* EditorIcons::GetUnknownFileIcon() {
	return _unknownFile.imtexture;
}

void* EditorIcons::GetDirIcon() {
	return _dirIcon.imtexture;
}

void* EditorIcons::GetBackBtnIcon() {
	return _backBtnIcon.imtexture;
}

void* EditorIcons::GetSceneIcon() {
	return _sceneIcon.imtexture;
}

void* EditorIcons::Get3DModelIcon() {
	return _3DModelIcon.imtexture;
}

void* EditorIcons::GetScriptIcon() {
	return _scriptIcon.imtexture;
}

void* EditorIcons::GetAudioIcon() {
	return _audioIcon.imtexture;
}

void* EditorIcons::GetPlayIcon() {
    return _playIcon.imtexture;
}

void* EditorIcons::GetPauseIcon() {
    return _pauseIcon.imtexture;
}

void* EditorIcons::GetStopIcon() {
    return _stopIcon.imtexture;
}

void* EditorIcons::GetTranslateIcon() {
    return _translateIcon.imtexture;
}

void* EditorIcons::GetScaleIcon() {
    return _scaleIcon.imtexture;
}

void* EditorIcons::GetRotateIcon() {
    return _rotateIcon.imtexture;
}