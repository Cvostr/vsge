#include "AngelApi.hpp"
#include <Scripting/Angel/AngelScriptLayer.hpp>
#include <Math/MathBase.hpp>
#include <Math/Vec3.hpp>
#include <Math/Quaternion.hpp>

using namespace VSGE;

float ctg(float a) {
	return 1.f / tan(a);
}

static void CreateVec3(float x, float y, float z, Vec3* ptr) {
	new (ptr) Vec3(x, y, z);
}

static void CreateQuat(float x, float y, float z, float w, Vec3* ptr) {
	new (ptr) Quat(x, y, z, w);
}

void VSGE::BindMath() {
	AngelScriptLayer* layer = AngelScriptLayer::Get();
	layer->RegisterObjectType(VEC3_TYPE_NAME, sizeof(Vec3), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<Vec3>());
	layer->RegisterObjectBehaviour(VEC3_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(new_as_obj_T<Vec3>), asCALL_CDECL_OBJLAST);
	layer->RegisterObjectBehaviour(VEC3_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION(CreateVec3), asCALL_CDECL_OBJLAST);
	layer->RegisterObjectBehaviour(VEC3_TYPE_NAME, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(del_as_obj_T<Vec3>), asCALL_CDECL_OBJLAST);
	layer->RegisterObjectProperty(VEC3_TYPE_NAME, "float x", offsetof(Vec3, x));
	layer->RegisterObjectProperty(VEC3_TYPE_NAME, "float y", offsetof(Vec3, y));
	layer->RegisterObjectProperty(VEC3_TYPE_NAME, "float z", offsetof(Vec3, z));
	//layer->RegisterObjectMethod(VEC3_TYPE_NAME, "Vec3 Normalize()", asMETHOD(Vec3, GetNormalized), asCALL_THISCALL);
	layer->RegisterObjectMethod(VEC3_TYPE_NAME, "void opAddAssign(Vec3 &in)", asMETHOD(Vec3, operator+=), asCALL_THISCALL);
	layer->RegisterObjectMethod(VEC3_TYPE_NAME, "void opAssign(Vec3 &in)", asMETHOD(Vec3, operator=), asCALL_THISCALL);
	//layer->RegisterObjectMethod(VEC3_TYPE_NAME, "Vec3 opMul(float) const", asMETHODPR(Vec3, operator*, (Vec3), float), asCALL_THISCALL);
	//layer->RegisterObjectMethod(VEC3_TYPE_NAME, "Vec3 opAdd(const Vec3&in) const", asMETHOD(Vec3, operator+), asCALL_THISCALL);
	//layer->RegisterObjectMethod(VEC3_TYPE_NAME, "bool opEquals(const Vec3&in) const", asMETHOD(Vec3, operator ==), asCALL_THISCALL);


	layer->RegisterObjectType(QUAT_TYPE_NAME, sizeof(Quat), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<Quat>());
	layer->RegisterObjectBehaviour(QUAT_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(new_as_obj_T<Quat>), asCALL_CDECL_OBJLAST);
	layer->RegisterObjectBehaviour(QUAT_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f(float, float, float, float)", asFUNCTION(CreateQuat), asCALL_CDECL_OBJLAST);
	layer->RegisterObjectBehaviour(QUAT_TYPE_NAME, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(del_as_obj_T<Quat>), asCALL_CDECL_OBJLAST);
	layer->RegisterObjectProperty(QUAT_TYPE_NAME, "float x", offsetof(Quat, x));
	layer->RegisterObjectProperty(QUAT_TYPE_NAME, "float y", offsetof(Quat, y));
	layer->RegisterObjectProperty(QUAT_TYPE_NAME, "float z", offsetof(Quat, z));
	layer->RegisterObjectProperty(QUAT_TYPE_NAME, "float w", offsetof(Quat, w));
	layer->RegisterObjectMethod(QUAT_TYPE_NAME, "void Normalize()", asMETHOD(Quat, GetNormalized), asCALL_THISCALL);
	layer->RegisterObjectMethod(QUAT_TYPE_NAME, "void opAssign(Quat &in)", asMETHOD(Quat, operator=), asCALL_THISCALL);

	layer->RegisterGlobalFunction("float sqrt(float)", asFUNCTION(sqrtf), asCALL_CDECL);
	layer->RegisterGlobalFunction("float sin(float)", asFUNCTION(sinf), asCALL_CDECL);
	layer->RegisterGlobalFunction("float cos(float)", asFUNCTION(cosf), asCALL_CDECL);
	layer->RegisterGlobalFunction("float tan(float)", asFUNCTION(tanf), asCALL_CDECL);
	layer->RegisterGlobalFunction("float ctg(float)", asFUNCTION(ctg), asCALL_CDECL);
	layer->RegisterGlobalFunction("float log(float)", asFUNCTION(logf), asCALL_CDECL);
	layer->RegisterGlobalFunction("float lg(float)", asFUNCTION(log10f), asCALL_CDECL);
	layer->RegisterGlobalFunction("float abs(float)", asFUNCTION(fabsf), asCALL_CDECL);
	layer->RegisterGlobalFunction("float rad(float)", asFUNCTION(to_radians), asCALL_CDECL);
}