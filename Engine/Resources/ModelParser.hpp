#pragma once

#include <Core/VarTypes/Base.hpp>
#include <Math/Mat4.hpp>
#include <Math/Quaternion.hpp>
#include <Math/Vertex.hpp>
#include <string>
#include <vector>
#include <Graphics/Mesh.hpp>

namespace VSGE {
    class SceneNode {
    private:
        std::string mNodeLabel;
        //Transform matrix
        Mat4 mNodeTransform;

        Vec3 node_translation;
        Vec3 node_scaling;
        Quat node_rotation;
    public:

        std::vector<std::string> child_node_labels;
        std::vector<SceneNode*> children;
        std::vector<std::string> mesh_names;

        SceneNode() {
        }
    };

    class SceneFileExport {
    private:
        std::vector<Mesh*> mMeshes;
        SceneNode* mRootNode;
    public:

        void pushMesh(Mesh* mesh);
        void setRootNode(SceneNode* node);
        void write(std::string output_file);
       // void writeNode(ZsStream* stream, SceneNode* node);
        //void getNodesNum(unsigned int* nds_ptr, SceneNode* node);

        SceneFileExport();
    };

    class ImportedSceneFile {
    private:
        std::vector<SceneNode*> nodes_list;
    public:
        std::vector<Mesh*> meshes_toRead;
        SceneNode* rootNode;

        SceneNode* getSceneNodeWithName(std::string label);
        void loadFromBuffer(byte* buffer, uint32 buf_size);
        void loadFromFile(std::string file);
        void clearMeshes();

        void makeNodeHierarchy(SceneNode* node);
        ImportedSceneFile();
        ~ImportedSceneFile();
    };

}