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
		static MonoLayer* _this;
		//main directory of mono
		std::string _mono_dir;
		//Path to compiler executable
		std::string _compiler_path;
		//mono domain object
		MonoDomain* _domain;

		bool CreateDomain();
	public:

		MonoLayer();

		static MonoLayer* Get() {
			return _this;
		}

		void SetMonoDir(std::string dir);

		MonoDomain* GetDomain();

		const std::string& GetCompilerPath();

		void CompileFile(const std::string& file_path, const std::string& out_path);

		void OnAttach();
		void OnUpdate();
		void OnDetach();
	};
}