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

#define MONO_ROOT_DOMAIN_NAME "VSGE_MONO_ROOT"
#define MONO_DOMAIN_NAME "VSGE_MONO"

namespace VSGE {
	class MonoScriptingLayer : public IApplicationLayer {
	private:
		static MonoScriptingLayer* _this;
		//mono domain object
		MonoDomain* _root_domain;
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

		MonoDomain* GetRootDomain();
		/// <summary>
		/// Creates Mono root AppDomain
		/// </summary>
		/// <returns></returns>
		bool CreateRootDomain();
		/// <summary>
		/// Release root domain, but it can't be initialized again
		/// </summary>
		void ReleaseRootDomain();

		MonoDomain* GetDomain();
		void CreateDomain();
		void ReleaseDomain();
	};
}