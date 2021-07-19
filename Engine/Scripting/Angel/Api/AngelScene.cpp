#include "../AngelScriptLayer.hpp"
#include "AngelApi.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Scene.hpp"

using namespace VSGE;

void VSGE::BindScene() {
	AngelScriptLayer* layer = AngelScriptLayer::Get();
	layer->RegisterObjectType(SCENE_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);
	layer->RegisterObjectBehaviour(SCENE_TYPE_NAME, asBEHAVE_FACTORY, "Scene@ f()", asFUNCTION(new_as_ref_T<Scene>), asCALL_CDECL);
	

	layer->RegisterObjectType(ENTITY_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);
	layer->RegisterObjectBehaviour(ENTITY_TYPE_NAME, asBEHAVE_FACTORY, "Entity@ f()", asFUNCTION(new_as_ref_T<Entity>), asCALL_CDECL);
	//layer->RegisterObjectProperty(ENTITY_TYPE_NAME, "int propsNum", offsetof(Engine::GameObject, props_num));
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "Scene@ GetScene() const", asMETHOD(Entity, GetScene), asCALL_THISCALL);
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "const string& GetName() const", asMETHOD(Entity, GetName), asCALL_THISCALL);
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "void SetName(const string& in)", asMETHOD(Entity, SetName), asCALL_THISCALL);
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "bool IsActive() const", asMETHOD(Entity, IsActive), asCALL_THISCALL);
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "void SetActive(bool)", asMETHOD(Entity, SetActive), asCALL_THISCALL);
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, MAINCLASS_TYPE_NAME + std::string("@ GetScriptObject(const string&in)"), asMETHOD(Entity, GetScriptObjectWithName), asCALL_THISCALL);
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "uint32 GetChildrenCount() const", asMETHOD(Entity, GetChildrenCount), asCALL_THISCALL);
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "Entity@ GetChild(uint)", asMETHOD(Entity, GetChild), asCALL_THISCALL);
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "void AddChild(Entity@)", asMETHODPR(Entity, AddChild, (Entity*, bool), void), asCALL_THISCALL);
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "void RemoveChild(Entity@)", asMETHODPR(Entity, RemoveChild, (Entity*), void), asCALL_THISCALL);
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "void SetViewMask(uint64)", asMETHOD(Entity, SetViewMask), asCALL_THISCALL);
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "uint64 GetViewMask()", asMETHOD(Entity, GetViewMask), asCALL_THISCALL);
	
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "const Vec3& GetPosition() const", asMETHOD(Entity, GetPosition), asCALL_THISCALL);
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "const Vec3& GetScale() const", asMETHOD(Entity, GetScale), asCALL_THISCALL);
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "const Quat& GetRotation() const", asMETHOD(Entity, GetRotation), asCALL_THISCALL);
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "void SetPosition(const Vec3& in)", asMETHOD(Entity, SetPosition), asCALL_THISCALL);
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "void SetScale(const Vec3& in)", asMETHOD(Entity, SetScale), asCALL_THISCALL);
	layer->RegisterObjectMethod(ENTITY_TYPE_NAME, "void SetRotation(const Quat& in)", asMETHOD(Entity, SetRotation), asCALL_THISCALL);

	
	layer->RegisterObjectMethod(SCENE_TYPE_NAME, "Entity@ GetEntityWithName(const string& in) const", asMETHOD(Scene, GetEntityWithName), asCALL_THISCALL);
}