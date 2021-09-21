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
}

void EditorIcons::FreeIcons() {
    _sampler.Destroy();
}