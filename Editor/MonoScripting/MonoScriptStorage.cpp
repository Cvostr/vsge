#include "MonoScriptStorage.hpp"
#include <filesystem>
#include <algorithm>
#include <Core/FileLoader.hpp>
#include <Windows/ConsoleWindow.hpp>
#include <EditorLayers/ImGuiLayer.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGEditor;
using namespace VSGE;
using namespace std::filesystem;

MonoScriptStorage* MonoScriptStorage::_this = new MonoScriptStorage();

MonoScript::MonoScript() {
	
}
MonoScript::~MonoScript() {

}

void MonoScript::SetFilePath(const std::string& file_path) {
	_abs_path = file_path;
	//Calculate class name
	uint32 last_slash_pos;
	for (uint32 i = 0; i < file_path.size(); i++) {
		if (file_path[i] == '\\' || file_path[i] == '/')
			last_slash_pos = i;
	}
	last_slash_pos++;
	while (file_path[last_slash_pos] != '.') {
		_class_name.push_back(file_path[last_slash_pos++]);
	}
}

const std::string& MonoScript::GetClassName() {
	return _class_name;
}

MonoScriptStorage::MonoScriptStorage() {
	_this = this;
	_is_scripting_ready = false;
	_compiler = new MonoScriptCompiler;
	_compiler->Run();
	_compiler->SetThreadName("mono-compile");

	_api_descrs = new MonoApiDescriptions;
}

MonoApiDescriptions* MonoScriptStorage::GetApiDescriptors() {
	return _api_descrs;
}

std::vector<MonoScript*>& MonoScriptStorage::GetScripts() {
	return _scripts;
}

const std::string& MonoScriptStorage::GetDllOutputPath() {
	return _compiler->GetOutputPath();
}

const std::string& MonoScriptStorage::GetApiDllPath() {
	return _compiler->GetApiDllPath();
}

bool MonoScriptStorage::IsScriptingReady() {
	return _is_scripting_ready;
}

void MonoScriptStorage::SetScriptingReady() {
	_is_scripting_ready = true;
}

MonoScript* MonoScriptStorage::GetScriptWithFilePath(const std::string& file_path) {
	for (auto& script : _scripts) {
		if (script->GetFilePath() == file_path)
			return script;
	}
	return nullptr;
}

void MonoScriptStorage::AddScript(const std::string& file_path) {
	bool exist = false;
	for (auto& script : _scripts) {
		if (script->GetFilePath() == file_path)
			exist = true;
	}

	if (!exist) {
		MonoScript* script = new MonoScript;
		script->SetFilePath(file_path);
		_scripts.push_back(script);
	}
}
void MonoScriptStorage::RemoveScript(const std::string& file_path) {
	MonoScript* script = GetScriptWithFilePath(file_path);
	if (script) {
		auto iter = std::remove(_scripts.begin(), _scripts.end(), script);
		_scripts.pop_back();
	}
}

void MonoScriptStorage::ClearScripts() {
	for (auto& script : _scripts) {
		delete script;
	}
	_scripts.clear();
}

void MonoScriptStorage::BuildScriptList(const std::string& root_dir) {
	for (const auto& entry : directory_iterator(root_dir)) {
		//if file starts with "." then don't show it
		if (entry.path().filename().string()[0] == '.')
			continue;
		if (entry.is_directory())
			BuildScriptList(entry.path().string());
		//if it is a file
		if (!entry.is_directory() && entry.path().extension().string() == ".cs") {
			AddScript(entry.path().string());
		}
	}
}

void MonoScriptStorage::Compile() {
	if (_scripts.size() == 0)
		return;

	MonoScriptingLayer::Get()->ReleaseDomain();
	MonoScriptingLayer::Get()->CreateDomain();

	ConsoleWindow* cw = ImGuiLayer::Get()->GetWindow<ConsoleWindow>();
	cw->ClearMessages(LogType::LOG_TYPE_SCRIPT_COMPILE_ERROR);
	_compiler->QueueCompilation();
	_is_scripting_ready = false;
}

void MonoScriptStorage::CompileAndWait() {
	Compile();
	while (true) {
		if (_compiler->IsCompilationDone())
			break;
	}
}