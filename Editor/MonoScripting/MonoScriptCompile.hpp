#pragma once

#include <string>
#include <Core/Threading/Thread.hpp>
#include <Core/Threading/Mutex.hpp>

namespace VSGE {

	enum COMPILATION_STATE {
		COMPILATION_STATE_QUEUED,
		COMPILATION_STATE_ON_PROCESS,
		COMPILATION_STATE_DONE
	};

	class MonoScriptCompiler : public Thread {
	private:
		std::string _output;
		Mutex* _mutex;
		COMPILATION_STATE _state;

		std::string GetCompilationCmd();
	public:

		MonoScriptCompiler();
		~MonoScriptCompiler();

		void THRFunc();

		void QueueCompilation();

		bool IsCompilationDone();
	};
}