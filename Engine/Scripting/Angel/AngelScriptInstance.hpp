#pragma once

#include "AngelScriptModule.hpp"

namespace VSGE {
	class Entity;

	class AngelScriptInstance {
	private:
		MainClassDesc* _class_desc;
		asIScriptObject* _mainclass_obj; //Object of main script class
	public:

		AngelScriptInstance();
		~AngelScriptInstance();

		void CreateInstance(MainClassDesc* class_desc, Entity* entity);
		void CreateInstance(const std::string& class_name, Entity* entity);

		asIScriptObject* GetMainClassObject();

		void CallOnStart();
		void CallOnStop();
		void CallOnUpdate();
		void CallOnTrigger(Entity* obj);
		void CallOnTriggerEnter(Entity* obj);
		void CallOnTriggerExit(Entity* obj);
	};
}