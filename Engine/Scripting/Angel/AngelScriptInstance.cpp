#include "AngelScriptInstance.hpp"

using namespace VSGE;

AngelScriptInstance::AngelScriptInstance() {

}

AngelScriptInstance::~AngelScriptInstance() {

}

asIScriptObject* AngelScriptInstance::GetMainClassObject() {
	return _mainclass_obj;
}

void AngelScriptInstance::CallOnStart() {

}
void AngelScriptInstance::CallOnStop() {

}
void AngelScriptInstance::CallOnUpdate() {

}