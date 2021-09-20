#pragma once

#include <Engine/ApplicationLayer.hpp>
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

#include "MonoScriptBlob.hpp"

namespace VSGE {
	class MonoScriptingLayer : public IApplicationLayer {
	private:
		static MonoScriptingLayer* _this;
		//mono domain object
		MonoDomain* _domain;
		MonoScriptBlob* _scripts_blob;
	public:

		MonoScriptingLayer();
		~MonoScriptingLayer();

		static MonoScriptingLayer* Get() {
			return _this;
		}

		MonoScriptBlob* GetScriptsBlob() {
			return _scripts_blob;
		}

		void OnAttach();
		void OnUpdate();
		void OnDetach();

		MonoDomain* GetDomain();
		bool CreateDomain();
		/// <summary>
		/// Release domain, but it can't be initialized again
		/// </summary>
		void ReleaseDomain();
	};
}