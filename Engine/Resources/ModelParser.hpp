#pragma once

#include <Core/VarTypes/Base.hpp>
#include <Math/Mat4.hpp>
#include <Math/Quaternion.hpp>
#include <Math/Vertex.hpp>
#include <string>
#include <vector>
#include <Graphics/Mesh.hpp>
#include <Graphics/Bone.hpp>
#include "Core/ByteSerialize.hpp"

namespace VSGE {

    class MeshContainer {
    public:
        std::string meshName;

        Vertex* vertexArray;
        VertexSkinningData* vertexSkinningArray;

        uint32* indexArray;
        Bone* bonesArray;

        uint32 vertexCount;
        uint32 indexCount;
        uint32 bonesCount;

        MeshContainer() :
            vertexArray(nullptr),
            vertexSkinningArray(nullptr),
            indexArray(nullptr),
            bonesArray(nullptr),

            vertexCount(0),
            indexCount(0),
            bonesCount(0)
        {}

        ~MeshContainer() {
            if (vertexArray)
                delete[] vertexArray;
            if (vertexSkinningArray)
                delete[] vertexSkinningArray;
            if (indexArray)
                delete[] indexArray;
            if (bonesArray)
                delete[] bonesArray;
        }
    };

    class SceneNode {
    private:
        std::string mNodeLabel;
        //Transform matrix
        Mat4 mNodeTransform;

        Vec3 node_translation;
        Vec3 node_scaling;
        Quat node_rotation;
    public:

        void SetNodeLabel(const std::string& label) {
            mNodeLabel = label;
        }

        const std::string GetLabel() {
            return mNodeLabel;
        }

        void SetNodeTransform(const Mat4& matrix) {
            mNodeTransform = matrix;
        }

        const Mat4& GetNodeTransform() {
            return mNodeTransform;
        }

        void SetTranslationScalingRotation(const Vec3& t, const Vec3& s, const Quat& r) {
            node_translation = t;
            node_scaling = s;
            node_rotation = r;
        }

        const Vec3& GetTranslation() {
            return node_translation;
        }

        const Vec3& GetScale() {
            return node_scaling;
        }

        const Quat& GetRotation() {
            return node_rotation;
        }

        std::vector<std::string> child_node_labels;
        std::vector<SceneNode*> children;
        std::vector<std::string> mesh_names;

        SceneNode() {
        }
    };

    class SceneFileExport {
    private:
        std::vector<MeshContainer*> mMeshes;
        SceneNode* mRootNode;
    public:

        void pushMesh(MeshContainer* mesh);
        void setRootNode(SceneNode* node);
        void write(std::string output_file);
        void writeNode(ByteSerialize* stream, SceneNode* node);
        void getNodesNum(uint32* nds_ptr, SceneNode* node);

        SceneFileExport() {
            mRootNode = nullptr;
        }
    };

    class ImportedSceneFile {
    private:
        std::vector<SceneNode*> nodes_list;
    public:
        std::vector<MeshContainer*> mReadedMeshes;
        SceneNode* rootNode;

        SceneNode* getSceneNodeWithName(std::string label);
        void loadFromBuffer(byte* buffer, uint32 buf_size);
        void loadFromFile(std::string file);
        void clearMeshes();

        void makeNodeHierarchy(SceneNode* node);
        ImportedSceneFile() {
            rootNode = nullptr;
        }
        ~ImportedSceneFile() {

        }
    };

}