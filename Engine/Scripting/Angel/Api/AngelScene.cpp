#include "../AngelScriptLayer.hpp"
#include "AngelApi.hpp"
#include "Scene/Entity.hpp"

using namespace VSGE;

void VSGE::BindScene() {
	AngelScriptLayer* layer = AngelScriptLayer::Get();
	layer->RegisterObjectType(ENTITY_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);
	layer->RegisterObjectBehaviour(ENTITY_TYPE_NAME, asBEHAVE_FACTORY, "Entity@ f()", asFUNCTION(new_as_ref_T<Entity>), asCALL_CDECL);

	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "void SetActive(bool)", asMETHOD(Entity, SetActive), asCALL_THISCALL);
	//layer->RegisterObjectProperty(ENTITY_TYPE_NAME, "int propsNum", offsetof(Engine::GameObject, props_num));
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "const string& GetName() const", asMETHOD(Entity, GetName), asCALL_THISCALL);
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "void SetName(const string& in)", asMETHOD(Entity, SetName), asCALL_THISCALL);
	//layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "ZPScript@ getScriptClass(const string&in)", asMETHOD(Engine::GameObject, getScriptObjectWithName), asCALL_THISCALL);
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "uint32 GetChildrenCount() const", asMETHOD(Entity, GetChildrenCount), asCALL_THISCALL);
	/*layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "Entity@ getChild(uint)", asMETHOD(Engine::GameObject, getChild), asCALL_THISCALL);
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "void addChild(GameObject@)", asMETHODPR(Engine::GameObject, addChildObject, (GameObject*, bool), void), asCALL_THISCALL);
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "void removeChild(GameObject@)", asMETHODPR(Engine::GameObject, removeChildObject, (GameObject*), void), asCALL_THISCALL);
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "void SetViewMask(uint64)", asMETHOD(Engine::GameObject, setViewMask), asCALL_THISCALL);
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "uint64 GetViewMask()", asMETHOD(Engine::GameObject, getViewMask), asCALL_THISCALL);
	*/
	}