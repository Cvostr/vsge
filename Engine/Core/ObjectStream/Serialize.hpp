#pragma once

#include "../VarTypes/Base.hpp"
#include <string>

namespace VSGE {

	enum SerializeType {
		SERIALIZE_TYPE_BINARY
	};

	class Serialize {
	private:
	public:
		template<typename T>
		void Serialize(const std::string& name, T& var);
	};
}