#pragma once

#include <string>
#include <vector>
#include <Core/VarTypes/Base.hpp>
#include <Math/Mat4.hpp>

namespace VSGE {

    class Bone {
    private:
        std::string m_boneName;
        Mat4 m_offsetMatrix;

        uint32 m_verticesAffected;
    public:

        uint32 GetAffectedVerticesCount() const;

        void SetOffsetMatrix(const Mat4& offsetMatrix);

        const Mat4& GetOffsetMatrix() const;

        const std::string GetName() const;

        Bone();

        Bone(const std::string& name, uint32 vertices);

        Bone(const std::string& name);
    };

    typedef std::vector<Bone> tBonesList;
}