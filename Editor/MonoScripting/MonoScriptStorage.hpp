#pragma once

#include <string>
#include <vector>
#include <Core/VarTypes/Base.hpp>
#include "MonoScriptCompile.hpp"

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
	};

	class MonoScriptStorage {
	private:
		std::vector<MonoScript*> _scripts;
		VSGE::MonoScriptCompiler* _compiler;
		static MonoScriptStorage* _this;
	public:

		MonoScriptStorage();

		static MonoScriptStorage* Get() {
			return _this;
		}

		std::vector<MonoScript*>& GetScripts();
		MonoScript* GetScriptWithFilePath(const std::string& file_path);
		void AddScript(const std::string& file_path);
		void RemoveScript(const std::string& file_path);
		void BuildScriptList(const std::string& root_dir);
		void ClearScripts();
		void Compile();
	};
}