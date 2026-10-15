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

        void SetNodeLabel(const std::string& label);
        const std::string& GetLabel() const;

        void SetNodeTransform(const Mat4& matrix);
        const Mat4& GetNodeTransform();

        const Vec3& GetTranslation();
        const Vec3& GetScale();
        const Quat& GetRotation();

        void SetTranslationScalingRotation(const Vec3& transform, const Vec3& scale, const Quat& rotation);

        std::vector<std::string> child_node_labels;
        std::vector<SceneNode*> children;
        std::vector<std::string> mesh_names;

        SceneNode();
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
        std::vector<MeshContainer*> mMeshes;
        SceneNode* rootNode;
    public:
  
        void loadFromBuffer(byte* buffer, uint32 buf_size);
        void loadFromFile(std::string file);

        const std::vector<MeshContainer*>& getMeshes() const;
        SceneNode* getSceneNodeWithName(std::string label);
        SceneNode* getRootNode() const;

        void makeNodeHierarchy(SceneNode* node);
        ImportedSceneFile();
        ~ImportedSceneFile();
    };

}