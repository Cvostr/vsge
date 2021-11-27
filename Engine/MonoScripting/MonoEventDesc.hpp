#pragma once

#include <mono/metadata/appdomain.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <string>

namespace VSGE {
	class MonoClassDesc;

	class MonoEventDesc {
	private:
		std::string _name;
		MonoEvent* _event;
		MonoMethod* _raise_method;
		MonoClassDesc* _class;
	public:
		MonoEventDesc(MonoClassDesc* class_desc, MonoEvent* mono_event);

		const std::string& GetName() const;
		MonoClassDesc* GetClass() const;
		void Create(MonoClassDesc* class_desc, MonoEvent* mono_event);
	};
}