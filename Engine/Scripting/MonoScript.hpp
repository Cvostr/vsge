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
        MonoObject* _object;
        //base class methods
        //MonoMethod* _start;
        //MonoMethod* _update;
    public:
        MonoScript();
        ~MonoScript();

        bool CreateFromBytes(const byte* data, uint32 size, const std::string& class_name);
	};
}