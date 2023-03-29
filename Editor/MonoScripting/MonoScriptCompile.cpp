#include "MonoScriptCompile.hpp"
#include "MonoScriptStorage.hpp"
#include <Core/Logger.hpp>
#include <Engine/Application.hpp>
#include <mpi/Os/Process.hpp>

using namespace VSGE;
using namespace VSGEditor;

MonoScriptCompiler::MonoScriptCompiler() :
    _mutex(new Mpi::Mutex),
    _state(COMPILATION_STATE_DONE),
    _compilation_error(false),
    _dll_output_path("mono_temp.dll"),
    _api_dll_path("api.dll")
{

}
MonoScriptCompiler::~MonoScriptCompiler() {
    SAFE_RELEASE(_mutex)
}

std::string MonoScriptCompiler::ExecuteCompilation() {
    MonoScriptStorage* storage = MonoScriptStorage::Get();
    std::string input;
    for (auto& script : storage->GetScripts()) {
        input += script->GetFilePath() + " ";
    }
    if (input.size() > 0)
        input.pop_back(); //remove space

    Mpi::ProcessCreateInfo compileProcCreateInfo = {};
    compileProcCreateInfo.flags = PROCESS_COMBINE_STDERR | PROCESS_REDIRECT_STREAMS;
#ifdef _WIN32
    compileProcCreateInfo.executable = "cmd.exe";
    compileProcCreateInfo.args.push_back("/C ..\\MonoScripting\\MonoBinaries\\bin\\csc.bat -target:library -nologo -reference:" + _api_dll_path + " -out:" + _dll_output_path + " " + input);
#endif
#ifdef __linux__
    compileProcCreateInfo.executable = "sh";
    compileProcCreateInfo.args.push_back("../MonoScripting/MonoBinaries/bin/csc.sh -target:library -nologo -reference:" + _api_dll_path + " -out:" + _dll_output_path + " " + input);
#endif

    std::string result;
    Mpi::Process compileProcess = Mpi::Process::createProcess(compileProcCreateInfo);
    Mpi::InputStream* processOut = compileProcess.getStdout();
    char buff[100];
    int len = 0;
    while ((len = processOut->read(buff, 100)) > 0) {
        for (int i = 0; i < len; i++) {
            result.push_back(buff[i]);
        }
    }

    compileProcess.waitFor();

    return result;
}

const std::string& MonoScriptCompiler::GetOutputPath() {
    return _dll_output_path;
}

const std::string& MonoScriptCompiler::GetApiDllPath() {
    return _api_dll_path;
}

void MonoScriptCompiler::THRFunc() {
    while (_running) {

        if (_state == COMPILATION_STATE_QUEUED) {
            _state = COMPILATION_STATE_ON_PROCESS;
            //Выполнить компиляцию
            _output = ExecuteCompilation();
            //disable logger output to stdout
            Logger::GetOpLogger().SetPrintToStdout(false);
            //Send logs with each error
            while (_output.find_first_of('\n') != std::string::npos) {
                uint32 pos = (uint32)_output.find_first_of('\n');
                std::string message = _output.substr(0, pos);
                if (message.find("error"))
                    _compilation_error = true;
                _output = _output.substr((uint64)pos + 1);
                Logger::Log(LogType::LOG_TYPE_SCRIPT_COMPILE_ERROR) << message << "\n";
            }
            _state = COMPILATION_STATE_DONE;
            //enable logger output to stdout
            Logger::GetOpLogger().SetPrintToStdout(true);
            ScriptCompilationDoneEvent* app_event = new ScriptCompilationDoneEvent;
            Application::Get()->QueueEvent(app_event);
        }
        SleepThread(100);
    }
}

void MonoScriptCompiler::QueueCompilation() {
    _mutex->lock();
    if (_state == COMPILATION_STATE_DONE) {
        _state = COMPILATION_STATE_QUEUED;
        _compilation_error = false;
    }
    _mutex->unlock();

    Application::Get()->OnEvent(ScriptCompilationBeginEvent());
}

bool MonoScriptCompiler::IsCompilationDone() {
    return _state == COMPILATION_STATE_DONE;
}

bool MonoScriptCompiler::IsCompilationError() {
    return _compilation_error;
}

bool MonoScriptCompiler::IsCompiling() {
    return _state == COMPILATION_STATE_QUEUED || _state == COMPILATION_STATE_ON_PROCESS;
}