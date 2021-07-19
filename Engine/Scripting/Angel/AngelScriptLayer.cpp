#include "AngelScriptLayer.hpp"
#include <Core/Logger.hpp>
#include "Api/AngelApi.hpp"
#include <angelscript/scriptstdstring.h>

using namespace VSGE;

AngelScriptLayer* AngelScriptLayer::_this = nullptr;

AngelScriptLayer::AngelScriptLayer() :
	_engine(nullptr),
	_context(nullptr),
	_module(new AngelScriptModule)
{
	_this = this;
}

AngelScriptLayer::~AngelScriptLayer() {
	DestroyEngine();
}

void AngelScriptLayer::CreateEngine() {
	_engine = asCreateScriptEngine();
	_engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
	_context = _engine->CreateContext();

	//Add base script class
	RegisterInterface(MAINCLASS_TYPE_NAME);
	RegisterStdString(_engine);
	//RegisterStdStringUtils(_engine);
	BindMath();
	BindIO();
	BindScene();

	_module->CreateModule(_engine, "vsge");
}

void AngelScriptLayer::DestroyEngine() {
	_context->Release();
	_engine->ShutDownAndRelease();

	SAFE_RELEASE(_module)
}

asIScriptEngine* AngelScriptLayer::GetScriptEngine() {
	return _engine;
}

asIScriptContext* AngelScriptLayer::GetScriptContext() {
	return _context;
}

AngelScriptModule* AngelScriptLayer::GetModule() {
	return _module;
}

void AngelScriptLayer::SetContextFunction(asIScriptFunction* function) {
	_context->Prepare(function);
}

int AngelScriptLayer::ContextExecute() {
	return _context->Execute();
}

void* AngelScriptLayer::GetAddressOfReturnValue() {
	return _context->GetAddressOfReturnValue();
}

void AngelScriptLayer::ContextSetObject(void* object) {
	_context->SetObject(object);
}

int AngelScriptLayer::ContextSetArgumentObject(uint32 slot, void* object) {
	return _context->SetArgObject(slot, object);
}

int AngelScriptLayer::RegisterInterface(const std::string& name) {
	return _engine->RegisterInterface(name.c_str());
}

int AngelScriptLayer::RegisterEnum(const std::string& type) {
	return _engine->RegisterEnum(type.c_str());
}

int AngelScriptLayer::RegisterEnumValue(const std::string& type, const std::string& name, int value) {
	return _engine->RegisterEnumValue(type.c_str(), name.c_str(), value);
}

int AngelScriptLayer::RegisterObjectType(const std::string& name, int byteSize, asDWORD flags) {
	return _engine->RegisterObjectType(name.c_str(), byteSize, flags);
}

int AngelScriptLayer::RegisterObjectMethod(const std::string& obj, std::string declaration, const asSFuncPtr& funcPointer, asDWORD callConv, void* auxiliary, int compositeOffset, bool isCompositeIndirect) {
	return _engine->RegisterObjectMethod(obj.c_str(), declaration.c_str(), funcPointer, callConv, auxiliary, compositeOffset, isCompositeIndirect);
}

int AngelScriptLayer::RegisterGlobalProperty(const std::string& declaration, void* pointer) {
	return _engine->RegisterGlobalProperty(declaration.c_str(), pointer);
}

int AngelScriptLayer::RegisterObjectProperty(const std::string& obj, const std::string& declaration, int byteOffset, int compositeOffset, bool isCompositeIndirect) {
	return _engine->RegisterObjectProperty(obj.c_str(), declaration.c_str(), byteOffset, compositeOffset, isCompositeIndirect);
}

int AngelScriptLayer::RegisterObjectBehaviour(const std::string& obj, asEBehaviours behaviour, const std::string& declaration, const asSFuncPtr& funcPointer, asDWORD callConv, void* auxiliary, int compositeOffset, bool isCompositeIndirect) {
	return _engine->RegisterObjectBehaviour(obj.c_str(), behaviour, declaration.c_str(), funcPointer, callConv, auxiliary, compositeOffset, isCompositeIndirect);
}

int AngelScriptLayer::RegisterGlobalFunction(const std::string& func_name,
	const asSFuncPtr& funcPointer,
	asDWORD 	callConv,
	void* auxiliary) {

	return _engine->RegisterGlobalFunction(func_name.c_str(), funcPointer, callConv, auxiliary);
}

void VSGE::MessageCallback(const asSMessageInfo* msg, void* param)
{
	std::string prefix;
	switch (msg->type)
	{
	case asMSGTYPE_WARNING:
		prefix = "WARN";
		break;
	case asMSGTYPE_ERROR:
		prefix = "ERROR";
		break;
	default:
		break;
	}

	std::string _msg = prefix + "(" + std::string(msg->section) + ") " + "(" + std::to_string(msg->row)
		+ ":" + std::to_string(msg->col) + ") " + msg->message;

	Logger::Log(LogType::LOG_TYPE_SCRIPT_COMPILE_ERROR) << _msg;
}