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

        uint32 GetAffectedVerticesCount() { return _verticesAffected; }

        void SetOffsetMatrix(const Mat4& offsetMatrix){
            _offsetMatrix = offsetMatrix;
        }

        const Mat4& GetOffsetMatrix() { return _offsetMatrix; }

        const std::string GetName() { return _boneName; }

        Bone() :
            _boneName(""),
            _verticesAffected(0)
        {

        }

        Bone(const std::string& name, uint32 vertices) :
            _boneName(name),
            _verticesAffected(vertices)
        {}

        Bone(const std::string& name) :
            _boneName(name),
            _verticesAffected(0)
        {}
    };

    typedef std::vector<Bone> tBonesList;
}