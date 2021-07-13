#pragma once 

#include <Engine/ApplicationLayer.hpp>
#include "AngelScriptModule.hpp"
#include <angelscript/angelscript.h>
#include <string>

namespace VSGE {
	class AngelScriptLayer : public IApplicationLayer {
	private:
        static AngelScriptLayer* _this;

		asIScriptEngine* _engine;
		asIScriptContext* _context;

        AngelScriptModule* _module;
	public:

        AngelScriptLayer();

        ~AngelScriptLayer();

        static AngelScriptLayer* Get() {
            return _this;
        }

        void OnAttach(){}

        void OnUpdate(){}

		void CreateEngine();

        void DestroyEngine();

		asIScriptEngine* GetScriptEngine();

		asIScriptContext* GetScriptContext();

        AngelScriptModule* GetModule();

        void SetContextFunction(asIScriptFunction* function);
        int ContextExecute();
        void* GetAddressOfReturnValue();
        void ContextSetObject(void* object);
        int ContextSetArgumentObject(uint32 slot, void* object);

        int RegisterInterface(const std::string& name);
        int RegisterEnum(const std::string& type);
        int RegisterEnumValue(const std::string& type, const std::string& name, int value);
        int RegisterObjectType(const std::string& name, int byteSize, asDWORD flags);
        int RegisterObjectMethod(const std::string& obj, std::string declaration, const asSFuncPtr& funcPointer, asDWORD callConv, void* auxiliary = 0, int compositeOffset = 0, bool isCompositeIndirect = false);
        
        int RegisterGlobalProperty(const std::string& declaration, void* pointer);
        int RegisterObjectProperty(const std::string& obj, const std::string& declaration, int byteOffset, int compositeOffset = 0, bool isCompositeIndirect = false);
        
        int RegisterObjectBehaviour(const std::string& obj, asEBehaviours behaviour, const std::string& declaration, const asSFuncPtr& funcPointer, asDWORD callConv, void* auxiliary = 0, int compositeOffset = 0, bool isCompositeIndirect = false);
        int RegisterGlobalFunction(const std::string& func_name,
            const asSFuncPtr& funcPointer,
            asDWORD 	callConv,
            void* auxiliary = 0);
            

	};

    void MessageCallback(const asSMessageInfo* msg, void* param);
}