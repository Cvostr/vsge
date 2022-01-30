#pragma once

#include <Engine/ApplicationLayer.hpp>
#include <Core/VarTypes/String.hpp>
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
#include <Networking/NetworkingEvents.hpp>

#define MONO_ROOT_DOMAIN_NAME "VSGE_MONO_ROOT"
#define MONO_DOMAIN_NAME "VSGE_MONO"

namespace VSGE {

	struct MonoEventSubscription {
		EventType event_type;
		MonoObject* mono_object;
		MonoMethodDescr* method_descr;
	};

	struct NetworkEventsState {
		uint32 _client_id;

		void* client_ptr;
		void* server_ptr;

		byte* data;
		uint32 data_size;
	};

	class MonoScriptingLayer : public IApplicationLayer {
	private:
		static MonoScriptingLayer* _this;
		//mono domain object
		MonoDomain* _root_domain;
		MonoDomain* _domain;
		MonoScriptBlob* _scripts_blob;

		std::vector<MonoEventSubscription> subs_events;
		NetworkEventsState _network_state;
	public:

		MonoScriptingLayer();
		~MonoScriptingLayer();

		static MonoScriptingLayer* Get() {
			return _this;
		}

		MonoScriptBlob* GetScriptsBlob();

		void OnAttach();
		void OnUpdate();
		void OnDetach();
		/// <summary>
		/// Get root mono domain
		/// </summary>
		/// <returns>root mono domain pointer</returns>
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
		/// <summary>
		/// Create utf8 mono string object
		/// </summary>
		/// <param name="str">- std string input</param>
		/// <returns>mono script object pointer</returns>
		MonoString* CreateMonoString(const std::string& str);
		/// <summary>
		/// Create utf32 mono string object
		/// </summary>
		/// <param name="str">- VSGE String input</param>
		/// <returns>mono script object pointer</returns>
		MonoString* CreateMonoString(const String& str);
		/// <summary>
		/// Get working mono domain
		/// </summary>
		/// <returns></returns>
		MonoDomain* GetDomain();
		/// <summary>
		/// Create Mono working domain
		/// </summary>
		void CreateDomain();
		/// <summary>
		/// Release Mono working domain
		/// </summary>
		void ReleaseDomain();
		/// <summary>
		/// Register other thread in mono
		/// Call this function from other thread to register
		/// </summary>
		void AttachThread();

		void OnEvent(const VSGE::IEvent& event);

		void OnClientConnectedToServer(const VSGE::NetworkClientConnectedEvent& event);
		void OnClientDisonnectedFromServer(const VSGE::NetworkClientDisconnectedEvent& event);

		void SubscribeToEvent(MonoObject* obj, EventType event_type, const std::string& method_name);
			
		NetworkEventsState& GetNetworkEventState();

		template<typename T> 
		static void AddInternalCall(const std::string& decl, T func){
			mono_add_internal_call(decl.c_str(), (const void*)func);
		}
	};
}