#pragma once

#include "AngelScript.hpp"
#include "AngelScriptModule.hpp"

namespace VSGE {
	class AngelScriptInstance {
	private:

		asIScriptObject* _mainclass_obj; //Object of main script class
	public:

		AngelScriptInstance();
		~AngelScriptInstance();

		void CreateInstance(MainClassDesc* class_desc);

		asIScriptObject* GetMainClassObject();

		void CallOnStart();
		void CallOnStop();
		void CallOnUpdate();
		//void CallOnTrigger(Engine::GameObject* obj);
		//void CallOnTriggerEnter(Engine::GameObject* obj);
		//void CallOnTriggerExit(Engine::GameObject* obj);
	};
}