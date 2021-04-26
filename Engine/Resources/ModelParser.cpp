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
            //solver.move(1);

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


void SceneFileExport::pushMesh(MeshContainer* mesh) {
    this->mMeshes.push_back(mesh);
}
void SceneFileExport::setRootNode(SceneNode* node) {
    this->mRootNode = node;
}
void SceneFileExport::write(std::string output_file) {
    ByteSerialize* serializer = new ByteSerialize;

    serializer->Serialize("zs3mscene\n", 11);
    uint32 model_ver = 1000;
    uint32 meshes_num = static_cast<uint32>(this->mMeshes.size());
    uint32 nodes_num = 0;
    getNodesNum(&nodes_num, this->mRootNode);

    serializer->Serialize(model_ver);
    serializer->Serialize(meshes_num);
    serializer->Serialize(nodes_num);
    serializer->Serialize("\n"); //Write divider
    //Write all nodes
    writeNode(serializer, mRootNode);

    //Iterate over all meshes
    for (uint32 mesh_i = 0; mesh_i < meshes_num; mesh_i++) {
        MeshContainer* mesh_ptr = this->mMeshes[mesh_i];
        //std::cout << "ZS3M: Writing Mesh " << mesh_ptr->mesh_label << std::endl;
        serializer->Serialize("_MESH ", 7);
        serializer->Serialize(mesh_ptr->meshName);

        //Write base numbers
        serializer->Serialize(mesh_ptr->vertexCount);
        serializer->Serialize(mesh_ptr->indexCount);
        serializer->Serialize(mesh_ptr->bonesCount);
        //stream << "\n"; //Write divider
        //Write all vertices
        for (uint32 v_i = 0; v_i < mesh_ptr->vertexCount; v_i++) {
            Vertex* v_ptr = &mesh_ptr->vertexArray[v_i];
            VertexSkinningData* vs_ptr = &mesh_ptr->vertexSkinningArray[v_i];
            //Write vertex vectors
            serializer->Serialize(v_ptr->pos);
            serializer->Serialize(v_ptr->uv);
            serializer->Serialize(v_ptr->normal);
            serializer->Serialize(v_ptr->tangent);
            serializer->Serialize(v_ptr->bitangent);
            serializer->Serialize(vs_ptr->bones_num);

         
            for (uint32 vb_i = 0; vb_i < vs_ptr->bones_num; vb_i++) {
                uint32 bone_id = vs_ptr->ids[vb_i];
                float b_weight = vs_ptr->weights[vb_i];
                //Write bone values
                serializer->Serialize(bone_id);
                serializer->Serialize(b_weight);
            }
        }
        for (uint32 ind_i = 0; ind_i < mesh_ptr->indexCount; ind_i++) {
            serializer->Serialize(mesh_ptr->indexArray[ind_i]);
        }
        serializer->Serialize("\n"); //Write divider

        for (uint32 b_i = 0; b_i < mesh_ptr->bonesCount; b_i++) {
            Bone* bone = &mesh_ptr->bonesArray[b_i];
            //Write bone name
            serializer->Serialize(bone->GetName());
            //Write offset matrix
            for (uint32 m_i = 0; m_i < 4; m_i++) {
                for (uint32 m_j = 0; m_j < 4; m_j++) {
                    float m_v = bone->GetOffsetMatrix().Values[m_i][m_j];
                    serializer->Serialize(m_v);
                }
            }
            serializer->Serialize("\n");
        }
        serializer->Serialize("\n");
    }
    //stream.close();
}

void SceneFileExport::writeNode(ByteSerialize* stream, SceneNode* node) {
    //Write node header
    stream->Serialize("_NODE ", 7);
    stream->Serialize(node->GetLabel());

    uint32 meshesNum = static_cast<uint32>(node->mesh_names.size());
    uint32 childrenNum = static_cast<uint32>(node->children.size());

    stream->Serialize(meshesNum);
    stream->Serialize(childrenNum);

    stream->Serialize("\n"); //Write divider

    for (uint32 mesh_i = 0; mesh_i < meshesNum; mesh_i++) {
        //Write mesh name
        stream->Serialize(node->mesh_names[mesh_i]);
    }
    for (uint32 ch_i = 0; ch_i < childrenNum; ch_i++) {
        //Write child node string
        stream->Serialize(node->children[ch_i]->GetLabel());
    }
    //Write node base matrix
    for (uint32 m_i = 0; m_i < 4; m_i++) {
        for (uint32 m_j = 0; m_j < 4; m_j++) {
            float m_v = node->GetNodeTransform().Values[m_i][m_j];
            stream->Serialize(m_v);
        }
    }
    stream->Serialize(node->GetTranslation().x);//Writing position X
    stream->Serialize(node->GetTranslation().y); //Writing position Y
    stream->Serialize(node->GetTranslation().z); //Writing position Z

    stream->Serialize(node->GetScale().x);//Writing scale X
    stream->Serialize(node->GetScale().y); //Writing scale Y
    stream->Serialize(node->GetScale().z); //Writing scale Z

    stream->Serialize(node->GetRotation().x);//Writing rotation X
    stream->Serialize(node->GetRotation().y); //Writing rotation Y
    stream->Serialize(node->GetRotation().z); //Writing rotation Z
    stream->Serialize(node->GetRotation().w); //Writing rotation W
    stream->Serialize("\n"); //Write divider
    //Write all children
    for (uint32 ch_i = 0; ch_i < (uint32)node->children.size(); ch_i++) {
        writeNode(stream, node->children[ch_i]);
    }
}

void SceneFileExport::getNodesNum(unsigned int* nds_ptr, SceneNode* node) {
    *nds_ptr += 1;
    //Write all children
    for (unsigned int ch_i = 0; ch_i < node->children.size(); ch_i++) {
        getNodesNum(nds_ptr, node->children[ch_i]);
    }
}