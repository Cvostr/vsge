#include "Bone.hpp"

using namespace VSGE;

uint32 Bone::GetAffectedVerticesCount() const
{ 
    return m_verticesAffected;
}

void Bone::SetOffsetMatrix(const Mat4& offsetMatrix) {
    m_offsetMatrix = offsetMatrix;
}

const Mat4& Bone::GetOffsetMatrix() const
{
    return m_offsetMatrix;
}

const std::string Bone::GetName() const
{ 
    return m_boneName; 
}

Bone::Bone() :
    m_boneName(""),
    m_verticesAffected(0)
{

}

Bone::Bone(const std::string& name, uint32 vertices) :
    m_boneName(name),
    m_verticesAffected(vertices)
{}

Bone::Bone(const std::string& name) :
    m_boneName(name),
    m_verticesAffected(0)
{}