#pragma once

#include <Core/VarTypes/Base.hpp>
#include <Math/Mat4.hpp>
#include <Math/Quaternion.hpp>
#include <Math/Vertex.hpp>
#include <string>
#include <vector>

namespace VSGE {
    class SceneNode {

    public:
        //Node name string
        std::string node_label;
        //Transform matrix
        Mat4 node_transform;

        Vec3 node_translation;
        Vec3 node_scaling;
        Quat node_rotation;

        std::vector<std::string> child_node_labels;
        std::vector<SceneNode*> children;
        std::vector<std::string> mesh_names;

        SceneNode() {
        }
    };
}