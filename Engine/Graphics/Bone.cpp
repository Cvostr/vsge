#include "Bone.hpp"

using namespace VSGE;

uint32 Bone::GetAffectedVerticesCount() { 
    return _verticesAffected; 
}

void Bone::SetOffsetMatrix(const Mat4& offsetMatrix) {
    _offsetMatrix = offsetMatrix;
}

const Mat4& Bone::GetOffsetMatrix() {
    return _offsetMatrix; 
}

const std::string Bone::GetName() { 
    return _boneName; 
}

Bone::Bone() :
    _boneName(""),
    _verticesAffected(0)
{

}

Bone::Bone(const std::string& name, uint32 vertices) :
    _boneName(name),
    _verticesAffected(vertices)
{}

Bone::Bone(const std::string& name) :
    _boneName(name),
    _verticesAffected(0)
{}