#pragma once

#include <Core/VarTypes/Base.hpp>
#include <mono/metadata/appdomain.h>
#include <string>

namespace VSGE {
	class MonoScript {
	private:
        byte* script_data;
        uint32 script_size;

        MonoAssembly* _assembly;
        MonoImage* _image;
        MonoClass* _main_class;
        //base class methods
        MonoMethod* _start_method;
        MonoMethod* _update_method;
    public:
        MonoScript();
        ~MonoScript();

        MonoClass* GetMainClass();

        bool CreateFromSourceFile(const std::string& source_path);
        bool CreateFromBytes(const byte* data, uint32 size, const std::string& class_name);
        MonoMethod* GetMethod(const std::string& method);
        MonoMethod* GetStartMethod();
        MonoMethod* GetUpdateMethod();
	};
}