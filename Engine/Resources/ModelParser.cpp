#include "ModelParser.hpp"
#include <Core/ByteSolver.hpp>

using namespace VSGE;

void ImportedSceneFile::loadFromBuffer(byte* buffer, uint32 buf_size) {
    ByteSolver solver(buffer);

    char header[10];
    solver.Copy(header, 9);
    header[9] = '\0'; //Terminate string

    if (strcmp(header, "zs3mscene") == 1) {
        //File's header is incorrect
        return;
    }

    uint32 model_ver = 0;
    uint32 meshes_num = 0;
    uint32 nodes_num = 0;
    
    solver.Copy(&model_ver);
    solver.Copy(&meshes_num);
    solver.Copy(&nodes_num);
    
    if (model_ver != 1000) {
        return;
    }

    char prefix[5];
    
    while (solver.end()) {
        solver.move(1);
        solver.Copy(prefix, 5);

        if (prefix[0] == '_' && prefix[1] == 'N' && prefix[2] == 'O' && prefix[3] == 'D' && prefix[4] == 'E') {
            solver.move(1);
            SceneNode* node = new SceneNode;

            
            //Read NODE label string
            std::string node_label = solver.ReadNextString();
            node->SetNodeLabel(node_label);

            uint32 meshesNum = 0;
            uint32 childrenNum = 0;

            //Read data
            solver.Copy(&meshesNum);
            solver.Copy(&childrenNum);
            solver.move(1);

            //iterate over all meshes, connected to this node
            for (uint32 mesh_i = 0; mesh_i < meshesNum; mesh_i++) {
                std::string mesh_label;
                //Read NODE label string
                mesh_label = solver.ReadNextString();
                //Put mesh name
                node->mesh_names.push_back(mesh_label);
            }
            //iterate over all children nodes, connected to this node
            for (uint32 ch_i = 0; ch_i < childrenNum; ch_i++) {
                //Read child name
                std::string child_node_label = solver.ReadNextString();
                //Put child name
                node->child_node_labels.push_back(child_node_label);
            }
            Mat4 node_transform;
            solver.Copy(&node_transform);
            node->SetNodeTransform(node_transform);

            Vec3 translation, scale;
            Quat rotation;

            solver.Copy(translation);
            solver.Copy(scale);
            solver.Copy(rotation);
            node->SetTranslationScalingRotation(translation, scale, rotation);

            //Add node to list
            nodes_list.push_back(node);
        }

        //We reached mesh
        if (prefix[0] == '_' && prefix[1] == 'M' && prefix[2] == 'E' && prefix[3] == 'S' && prefix[4] == 'H') {
            solver.move(1);

            std::string mesh_label = solver.ReadNextString();
            //Read mesh label string

            int vertexNum = 0;
            int indexNum = 0;
            uint32 bonesNum = 0;
            //Read mesh info
            //Read vertex amount
            solver.Copy(vertexNum);
            solver.Copy(indexNum);
            solver.Copy(bonesNum);
            solver.move(1);

            MeshContainer* mesh = new MeshContainer;
            mesh->meshName = mesh_label;
            mesh->vertexCount = vertexNum;
            mesh->indexCount = indexNum;
            mesh->bonesCount = bonesNum;
            //Allocate arrays for vectors
            mesh->vertexArray = new Vertex[static_cast<uint32>(vertexNum)];
            mesh->vertexSkinningArray = new VertexSkinningData[static_cast<uint32>(vertexNum)];

            mesh->indexArray = new uint32[static_cast<uint32>(indexNum)];
            mesh->bonesArray = new Bone[bonesNum];
            
            for (uint32 v_i = 0; v_i < static_cast<uint32>(vertexNum); v_i++) {
                Vertex v_ptr;
                //Read vertex vectors
                solver.Copy(v_ptr.pos);
                solver.Copy(v_ptr.uv);
                solver.Copy(v_ptr.normal);
                solver.Copy(v_ptr.tangent);
                solver.Copy(v_ptr.bitangent);
                
                VertexSkinningData vertexSkinning;
                solver.Copy(vertexSkinning.bones_num);

                //Read vertex bone data
                for (uint32 vb_i = 0; vb_i < vertexSkinning.bones_num; vb_i++) {
                    uint32* bone_id = &vertexSkinning.ids[vb_i];
                    float* b_weight = &vertexSkinning.weights[vb_i];
                    //Read bone values
                    solver.Copy(bone_id);
                    solver.Copy(b_weight);
                }
                
                mesh->vertexArray[v_i] = v_ptr;
                mesh->vertexSkinningArray[v_i] = vertexSkinning;
            }
            //Read mesh indices
            for (uint32 in_i = 0; in_i < static_cast<uint32>(indexNum); in_i++) {
                solver.Copy(mesh->indexArray[in_i]);
            }
            solver.move(1);
            /*

            newmesh->Init();
            newmesh->setMeshData(newmesh->vertices_arr, newmesh->indices_arr, newmesh->mVerticesNum, newmesh->mIndicesNum);
            //newmesh->mBoundingBox.CreateFromVertexArray(newmesh->vertices_arr, newmesh->mVerticesNum);

            delete[] newmesh->vertices_arr;
            delete[] newmesh->indices_arr;
            newmesh->vertices_arr = 0x0;
            newmesh->indices_arr = 0x0;
            */
            for (uint32 b_i = 0; b_i < bonesNum; b_i++) {
                //Read mesh label string
                std::string bone_label = solver.ReadNextString();

                Bone bone(bone_label);
                Mat4 boneTransform;
                solver.Copy(&boneTransform);
                bone.SetOffsetMatrix(boneTransform);
                
                mesh->bonesArray[b_i] = bone;
                solver.move(1);
            }
            
            //Push mesh as output result
            this->mReadedMeshes.push_back(mesh);
        }
    }

    rootNode = nodes_list[0];

    makeNodeHierarchy(rootNode);
}

void ImportedSceneFile::makeNodeHierarchy(SceneNode* node) {
    for (unsigned int i = 0; i < node->child_node_labels.size(); i++) {
        std::string child_name = node->child_node_labels[i];

        SceneNode* nn = getSceneNodeWithName(child_name);

        if (nn == nullptr)
            continue;

        node->children.push_back(nn);

        makeNodeHierarchy(node->children[i]);
    }
}