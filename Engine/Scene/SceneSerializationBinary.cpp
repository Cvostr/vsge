#include "SceneSerialization.hpp"

using namespace VSGE;

void SceneSerializer::SerializeBinary(const std::string& path) {
	ByteSerialize serializer;

	serializer.WriteBytes("vsgebscene", 11);
	

}