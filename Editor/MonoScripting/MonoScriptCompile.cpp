#include "MonoScriptCompile.hpp"
#include <System/Shell.hpp>
#include "MonoScriptStorage.hpp"
#include <Core/Logger.hpp>
#include <Engine/Application.hpp>

using namespace VSGE;
using namespace VSGEditor;

MonoScriptCompiler::MonoScriptCompiler() :
	_mutex(new Mutex),
	_state(COMPILATION_STATE_DONE),
    _compilation_error(false)
{

}
MonoScriptCompiler::~MonoScriptCompiler() {

}

std::string MonoScriptCompiler::GetCompilationCmd() {
    MonoScriptStorage* storage = MonoScriptStorage::Get();
    std::string input;
    for (auto& script : storage->GetScripts()) {
        input += script->GetFilePath() + " ";
    }
    if(input.size() > 0)
        input.pop_back(); //remove space

    return "..\\MonoScripting\\MonoBinaries\\roslyn\\csc.exe -target:library -nologo -reference:..\\MonoScripting\\api.dll -out:mono_temp.dll " + input;
}

void MonoScriptCompiler::THRFunc() {
    while (mShouldRun) {

        if (_state == COMPILATION_STATE_QUEUED) {
            _state = COMPILATION_STATE_ON_PROCESS;
            _mutex->Lock();
            //Obtain pointer to LoadRequest
            std::string cmd = GetCompilationCmd();
            _output = ExecuteShellCommand(cmd);

            while (_output.find_first_of('\n') != std::string::npos) {
                uint32 pos = (uint32)_output.find_first_of('\n');
                std::string message = _output.substr(0, pos);
                if (message.find("error"))
                    _compilation_error = true;
                _output = _output.substr(pos + 1);
                Logger::Log(LogType::LOG_TYPE_SCRIPT_COMPILE_ERROR) << message << "\n";
            }
            _state = COMPILATION_STATE_DONE;

            ScriptCompilationDoneEvent* app_event = new ScriptCompilationDoneEvent;
            Application::Get()->QueueEvent(app_event);

            //unlock thread
            _mutex->Release();
        }
        SleepThread(100);
    }
}

void MonoScriptCompiler::QueueCompilation() {
	_mutex->Lock();
    if (_state == COMPILATION_STATE_DONE) {
        _state = COMPILATION_STATE_QUEUED;
        _compilation_error = false;
    }
	_mutex->Release();

    Application::Get()->OnEvent(ScriptCompilationBeginEvent());
}

bool MonoScriptCompiler::IsCompilationDone() {
    return _state == COMPILATION_STATE_DONE;
}

bool MonoScriptCompiler::IsCompilationError() {
    return _compilation_error;
}