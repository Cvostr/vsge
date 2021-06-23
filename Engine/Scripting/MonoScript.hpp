#pragma once

#include <Core/VarTypes/Base.hpp>
#include <mono/metadata/appdomain.h>
#include <string>

namespace VSGE {
	class MonoScript {
	private:
        MonoAssembly* _assembly;
        MonoImage* _image;
        MonoClass* _main_class;
        //base class methods
        //MonoMethod* _start;
        //MonoMethod* _update;
    public:
        MonoScript();
        ~MonoScript();

        MonoClass* GetMainClass();

        bool CreateFromSourceFile(const std::string& source_path);
        bool CreateFromBytes(const byte* data, uint32 size, const std::string& class_name);
	};
}