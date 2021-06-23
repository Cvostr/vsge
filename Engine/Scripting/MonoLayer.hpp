#pragma once

#include "../Engine/ApplicationLayer.hpp"
#include <string>
//mono headers
#include <mono/metadata/appdomain.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/loader.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/environment.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/exception.h>

namespace VSGE {
	class MonoLayer : public IApplicationLayer {
	private:
		std::string _mono_dir;

		std::string _compiler_path;

		MonoDomain* _domain;

		bool CreateDomain();
	public:

		void SetMonoDir(std::string dir);
		const std::string& GetCompilerPath() {
			return _compiler_path;
		}

		void OnAttach();
		void OnUpdate();
		void OnDetach();
	};
}