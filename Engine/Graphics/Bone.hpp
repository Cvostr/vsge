#pragma once

#include <string>
#include <vector>
#include <Core/VarTypes/Base.hpp>
#include <Math/Mat4.hpp>

namespace VSGE {

    class Bone {
    private:
        std::string _boneName;
        Mat4 _offsetMatrix;

        uint32 _verticesAffected;
    public:

        uint32 GetAffectedVerticesCount();

        void SetOffsetMatrix(const Mat4& offsetMatrix);

        const Mat4& GetOffsetMatrix();

        const std::string GetName();

        Bone();

        Bone(const std::string& name, uint32 vertices);

        Bone(const std::string& name);
    };

    typedef std::vector<Bone> tBonesList;
}