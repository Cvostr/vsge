#pragma once

#include <string>
#include <Core/Threading/Thread.hpp>
#include <mpi/Os/Mutex.hpp>
#include <Engine/Event.hpp>

namespace VSGE {

	enum COMPILATION_STATE {
		COMPILATION_STATE_QUEUED,
		COMPILATION_STATE_ON_PROCESS,
		COMPILATION_STATE_DONE
	};

	class ScriptCompilationBeginEvent : public IEvent {
	public:
		EVENT_CLASS_TYPE(EventType::EventScriptCompilationBegin)
		ScriptCompilationBeginEvent() {}
	};

	class ScriptCompilationDoneEvent : public IEvent {
	public:
		EVENT_CLASS_TYPE(EventType::EventScriptCompilationDone)
		ScriptCompilationDoneEvent() {}
	};

	class MonoScriptCompiler : public Thread {
	private:
		std::string _output;
		std::string _dll_output_path;
		std::string _api_dll_path;
		Mpi::Mutex * _mutex;
		COMPILATION_STATE _state;
		bool _compilation_error;

		std::string ExecuteCompilation();
	public:

		MonoScriptCompiler();
		~MonoScriptCompiler();

		void THRFunc();

		void QueueCompilation();

		bool IsCompiling();

		bool IsCompilationDone();

		bool IsCompilationError();

		const std::string& GetOutputPath();

		const std::string& GetApiDllPath();
	};
}