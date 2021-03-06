#pragma once

#include <string>
#include <vector>
#include <Core/VarTypes/Base.hpp>
#include "MonoScriptCompile.hpp"
#include "MonoApiDescriptions.hpp"

namespace VSGEditor {

	class MonoScript {
	private:
		std::string _abs_path;
		std::string _class_name;
	public:

		MonoScript();
		~MonoScript();

		const std::string& GetFilePath() {
			return _abs_path;
		}
		void SetFilePath(const std::string& file_path);

		const std::string& GetClassName();
	};

	class MonoScriptStorage {
	private:
		std::vector<MonoScript*> _scripts;
		VSGE::MonoScriptCompiler* _compiler;
		MonoApiDescriptions* _api_descrs;
		static MonoScriptStorage* _this;
		bool _is_scripting_ready;
	public:

		MonoScriptStorage();

		static MonoScriptStorage* Get() {
			return _this;
		}

		MonoApiDescriptions* GetApiDescriptors();
		std::vector<MonoScript*>& GetScripts();
		const std::string& GetDllOutputPath();
		const std::string& GetApiDllPath();
		MonoScript* GetScriptWithFilePath(const std::string& file_path);
		void AddScript(const std::string& file_path);
		void RemoveScript(const std::string& file_path);
		void BuildScriptList(const std::string& root_dir);
		void ClearScripts();
		void Compile();
		void CompileAndWait();
		bool IsScriptingReady();
		void SetScriptingReady();
	};
}