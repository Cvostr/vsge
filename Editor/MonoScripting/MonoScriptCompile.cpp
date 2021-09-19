#include "MonoScriptCompile.hpp"
#include <System/Shell.hpp>
#include "MonoScriptStorage.hpp"
#include <Core/Logger.hpp>

using namespace VSGE;

MonoScriptCompiler::MonoScriptCompiler() :
	_mutex(new Mutex),
	_state(COMPILATION_STATE_DONE)
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

    return "..\\MonoScripting\\MonoBinaries\\roslyn\\csc.exe -target:library -nologo -reference:..\\MonoScripting\\api.dll -out:D:\\compiled.dll " + input;
}

void MonoScriptCompiler::THRFunc() {
    while (mShouldRun) {

        if (_state == COMPILATION_STATE_QUEUED) {
            _state = COMPILATION_STATE_ON_PROCESS;
            _mutex->Lock();
            //Obtain pointer to LoadRequest
            std::string cmd = GetCompilationCmd();
            _output = ExecuteShellCommand(cmd);
            Logger::Log(LogType::LOG_TYPE_SCRIPT_COMPILE_ERROR) << _output;
            //unlock thread
            _mutex->Release();
        }
        SleepThread(100);
    }
}

void MonoScriptCompiler::QueueCompilation() {
	_mutex->Lock();
	if (_state == COMPILATION_STATE_DONE)
		_state = COMPILATION_STATE_QUEUED;
	_mutex->Release();
}

bool MonoScriptCompiler::IsCompilationDone() {
    return _state == COMPILATION_STATE_DONE;
}