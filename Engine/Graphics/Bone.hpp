#pragma once

#include <string>
#include <Core/VarTypes/Base.hpp>
#include <Math/Mat4.hpp>

namespace VSGE {

    class Bone {
    private:
        std::string mBoneName;
        Mat4 mOffsetMatrix;

        uint32 mVerticesAffected;
    public:

        uint32 GetAffectedVerticesCount() { return vertices_affected; }

        void SetOffsetMatrix(const Mat4& offsetMatrix){
            mOffsetMatrix = offsetMatrix;
        }

        const Mat4& GetOffsetMatrix() { return mOffsetMatrix; }

        const std::string GetName() { return mBoneName; }

        Bone(const std::string& name, uint32 vertices) :
            mBoneName(name),
            mVerticesAffected(vertices)
        {}

        Bone(const std::string& name) :
            mBoneName(name),
            mVerticesAffected(0)
        {}
    };

}