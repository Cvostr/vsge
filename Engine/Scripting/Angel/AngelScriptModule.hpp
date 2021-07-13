#pragma once

#include <Core/VarTypes/Base.hpp>
#include <angelscript/angelscript.h>
#include <angelscript/scriptbuilder.h>
#include <Core/VarTypes/MultitypeValue.hpp>
#include "stdio.h"
#include <string>

namespace VSGE {
    struct ClassFieldDesc {
        uint32 index;
        std::string name;
        int typeID;
        bool isPrivate;
        bool isProtected;
        bool isReference;
        MultitypeValue value;

        ClassFieldDesc() :
            index(0),
            typeID(0),
            isPrivate(false),
            isProtected(false),
            isReference(false)
        {
        }
    };

    class MainClassDesc {
    public:
        std::string _name;
        asITypeInfo* _info;
        uint32 _classFieldsNum;
        std::vector<ClassFieldDesc> _fields;

        asIScriptFunction* GetFuncOnMainClass(const std::string& decl);
        ClassFieldDesc* GetSuitableDesc(const std::string& name, int typeID, uint32 index);

        MainClassDesc() :
            _name(""),
            _info(nullptr),
            _classFieldsNum(0) {}
        ~MainClassDesc() {
            _fields.clear();
        }
    };

    typedef std::vector<MainClassDesc*> tMainClassDescs;

    class BytecodeStream : public asIBinaryStream {
    private:
        FILE* f;

        byte* bytecode;
        uint32 bytecode_size;
    public:

        void open_for_write(const std::string& output_file);
        void close();
        int Write(const void* ptr, asUINT size);

        void open_for_read(byte* bytecode, uint32 size);
        int Read(void* ptr, asUINT size);
    };

    class Entity;

	class AngelScriptModule {
	private:
		asIScriptModule* _module;

		CScriptBuilder _builder;
		BytecodeStream* _streamWriter;

        bool _compileFailed;

        tMainClassDescs _classInfos;
        void UpdateClassInfos();

	public:

        AngelScriptModule(asIScriptEngine* engine, const std::string& name);
        AngelScriptModule();

        ~AngelScriptModule();

        void CreateModule(asIScriptEngine* engine, const std::string& name);
        void Compile();
        bool IsCompileFailed();
        void SaveByteCode(const std::string dst_file);
        void LoadByteCode(byte* bytecode, uint32 size);
        void AddScript(byte* data, uint32 size, std::string label = "");
        MainClassDesc* GetMainClassDescByName(const std::string& className);
        
        tMainClassDescs& GetMainClassDescs() {
            return _classInfos;
        }

        int CreateMainClassObject(const std::string& className, Entity* entity, asIScriptObject** ClassObj);
        int CreateMainClassObject(MainClassDesc* desc, Entity* entity, asIScriptObject** ClassObj);
    };

    int onInclude(const char* include, const char* from, CScriptBuilder* builder, void* userParam);
}