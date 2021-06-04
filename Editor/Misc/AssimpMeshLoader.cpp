#include "AssimpMeshLoader.h"
#include <cassert>

static Assimp::Importer importer;
static unsigned int loadflags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices;
static unsigned int loadflagsAnim = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices;

#include <iostream>

using namespace VSGEditor;
using namespace VSGE;

void VSGEditor::cmat(aiMatrix4x4 matin, Mat4* matout){
    for (uint32 i = 0; i < 4; i++)
        for (uint32 j = 0; j < 4; j++)
            matout->Values[i][j] = matin[i][j];
}

bool VSGEditor::getSizes(std::string file_path, unsigned int* meshes, unsigned int* anims, unsigned int* textures, unsigned int* materials){
    const aiScene* scene = importer.ReadFile(file_path, 0);

    if(scene == nullptr) return false;

    *meshes = scene->mNumMeshes;
    *anims = scene->mNumAnimations;

    if(textures != nullptr)
        *textures = scene->mNumTextures;

    if(materials != nullptr)
        *materials = scene->mNumMaterials;

    importer.FreeScene();
    return true;
}


void VSGEditor::processMesh(aiMesh* mesh, MeshContainer* mesh_ptr) {
    const char* mnam = mesh->mName.C_Str();
    mesh_ptr->meshName = std::string(mnam);

    mesh_ptr->vertexCount = mesh->mNumVertices;
    mesh_ptr->indexCount = mesh->mNumFaces * 3;
    mesh_ptr->bonesCount = mesh->mNumBones;
    //Allocate memory for vertices and indices
    mesh_ptr->vertexArray = new Vertex[mesh_ptr->vertexCount];
    mesh_ptr->vertexSkinningArray = new VertexSkinningData[mesh_ptr->vertexCount];
    mesh_ptr->bonesArray = new Bone[mesh_ptr->bonesCount];
    mesh_ptr->indexArray = new uint32[mesh_ptr->indexCount];

    //Iterate over all vertices, read data and write to array
    for (uint32 v = 0; v < mesh_ptr->vertexCount; v++) {
        aiVector3D vertex_pos = mesh->mVertices[v];
        aiVector3D vertex_normal = mesh->mNormals[v];
        aiVector3D vertex_tangent = mesh->mTangents[v];
        aiVector3D vertex_bitangent = mesh->mBitangents[v];

        float U = mesh->mTextureCoords[0][v].x;
        float V = mesh->mTextureCoords[0][v].y;
        //Set data to vertex
        mesh_ptr->vertexArray[v] = Vertex(Vec3(vertex_pos.x, vertex_pos.y, vertex_pos.z), Vec2(U, V),
            Vec3(vertex_normal.x, vertex_normal.y, vertex_normal.z).GetNormalized(), Vec3(vertex_tangent.x, vertex_tangent.y, vertex_tangent.z),
            Vec3(vertex_bitangent.x, vertex_bitangent.y, vertex_bitangent.z)
        );

        //Read bones
        mesh_ptr->vertexSkinningArray[v].bones_num = 0;
        for(uint32 vw_i = 0; vw_i < MAX_BONE_PER_VERTEX; vw_i ++){
            mesh_ptr->vertexSkinningArray[v].ids[vw_i] = 0;
            mesh_ptr->vertexSkinningArray[v].weights[vw_i] = 0.f;
        }
    }

    for(uint32 bone_i = 0; bone_i < mesh_ptr->bonesCount; bone_i ++){
        aiBone* bone_ptr = mesh->mBones[bone_i];
        std::string bone_label_strstd = std::string(bone_ptr->mName.C_Str());
        Bone bone(bone_label_strstd, bone_ptr->mNumWeights);
        //Convert assimp bone offset matrix and store it in bone class
        Mat4 boneTransform;
        cmat(bone_ptr->mOffsetMatrix, &boneTransform);
        bone.SetOffsetMatrix(boneTransform);

        //Iterate over all weights to set them to vertices
        for(uint32 vw_i = 0; vw_i < bone.GetAffectedVerticesCount(); vw_i ++){
            aiVertexWeight* vw = &bone_ptr->mWeights[vw_i];
            //ZSVERTEX* vertex = &mesh_ptr->vertices_arr[vw->mVertexId];
            VertexSkinningData* vsd = &mesh_ptr->vertexSkinningArray[vw->mVertexId];

            if(vsd->bones_num + 1 > MAX_BONE_PER_VERTEX)
                //Its better way to crash here
                assert(0);

            //Add bone ID
            vsd->ids[vsd->bones_num] = bone_i;
            //Set bone weight
            vsd->weights[vsd->bones_num] = vw->mWeight;
            //Increase bones amount
            vsd->bones_num += 1;
        }

        //Push bone back
        mesh_ptr->bonesArray[bone_i] = bone;
    }

    for (uint32 i = 0; i < mesh_ptr->indexCount / 3; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (uint32 j = 0; j < face.mNumIndices; j++)
            mesh_ptr->indexArray[i * 3 + j] = face.mIndices[j];
    }
}


void VSGEditor::loadMesh(std::string file_path, MeshContainer* mesh_ptr, int index){
    const aiScene* scene = importer.ReadFile(file_path, loadflags);
    std::cout << "Loading mesh " << scene->mMeshes[index]->mName.C_Str() << " from file " << file_path << std::endl;

    processMesh(scene->mMeshes[index], mesh_ptr);
    importer.FreeScene();
}

void VSGEditor::loadAnimation(std::string file_path, Animation* anim, int index){
    const aiScene* scene = importer.ReadFile(file_path, loadflagsAnim);
    std::cout << "Loading animation " << scene->mAnimations[index]->mName.C_Str() << " from file " << file_path << std::endl;

    aiAnimation* _anim = scene->mAnimations[index];
    anim->SetName(std::string(_anim->mName.C_Str()));
    
    //Check, if animation name is empty
    /*if(anim->name.empty()){
        std::string postfix;
        genRandomString(&postfix, 3);
        anim->name = "Animation_" + postfix;
    }*/

    anim->SetDuration(_anim->mDuration);
    anim->SetTPS(_anim->mTicksPerSecond);
    anim->SetNumChannels(_anim->mNumChannels);


    for(uint32 chan_i = 0; chan_i < anim->GetNumChannels(); chan_i ++){
        //Assimp channel
        aiNodeAnim* ai_channel = _anim->mChannels[chan_i];
        //ZSPIRE channel
        AnimationChannel* channel = &anim->GetChannels()[chan_i];
        channel->anim_ptr = anim; //Set pointer
        channel->SetBoneName(std::string(ai_channel->mNodeName.C_Str()));

        channel->SetPositionKeysCount(ai_channel->mNumPositionKeys);
        channel->SetScaleKeysCount(ai_channel->mNumScalingKeys);
        channel->SetRotationKeysCount(ai_channel->mNumRotationKeys);

        //Write all transform data
        for(uint32 pos_k_i = 0; pos_k_i < channel->GetPosValues()->keysCount; pos_k_i ++){
            aiVector3D ai_pos_key = ai_channel->mPositionKeys[pos_k_i].mValue;
            channel->GetPosValues()->values[pos_k_i] = Vec3(ai_pos_key.x, ai_pos_key.y, ai_pos_key.z);

            channel->GetPosValues()->times[pos_k_i] = ai_channel->mPositionKeys[pos_k_i].mTime;
        }
        for(uint32 scale_k_i = 0; scale_k_i < channel->GetScaleValues()->keysCount; scale_k_i ++){
            aiVector3D ai_scale_key = ai_channel->mScalingKeys[scale_k_i].mValue;
            channel->GetScaleValues()->values[scale_k_i] = Vec3(ai_scale_key.x, ai_scale_key.y, ai_scale_key.z);

            channel->GetScaleValues()->times[scale_k_i] = ai_channel->mScalingKeys[scale_k_i].mTime;
        }
        for(uint32 rot_k_i = 0; rot_k_i < channel->GetRotationValues()->keysCount; rot_k_i ++){
            aiQuaternion ai_rot_key = ai_channel->mRotationKeys[rot_k_i].mValue.Normalize();
            channel->GetRotationValues()->values[rot_k_i] = Quat(ai_rot_key.x, ai_rot_key.y, ai_rot_key.z, ai_rot_key.w);

            channel->GetRotationValues()->times[rot_k_i] = ai_channel->mRotationKeys[rot_k_i].mTime;
        }

    }
    importer.FreeScene();
}

void VSGEditor::loadMaterial(std::string file_path, aiMaterial* material, int index) {
    const aiScene* scene = importer.ReadFile(file_path, loadflags);

    *material = *(scene->mMaterials[index]);

    importer.FreeScene();
}

void VSGEditor::loadNodeTree(std::string file_path, SceneNode* node){
    const aiScene* scene = importer.ReadFile(file_path, loadflags);

    SceneNode* root_node = new SceneNode;
    processNodeForTree(root_node, scene->mRootNode, scene);

    *node = *root_node;
    delete root_node;
    importer.FreeScene();
}


void VSGEditor::processNodeForTree(SceneNode* node, aiNode* node_assimp, const aiScene* scene){
    node->SetNodeLabel(node_assimp->mName.C_Str()); //assigning node name

    aiMatrix4x4 node_transform = node_assimp->mTransformation;
    //Write node matrix
    Mat4 nodeTransform;
    cmat(node_transform, &nodeTransform);
    node->SetNodeTransform(nodeTransform);

    aiVector3D translation, scale;
    aiQuaternion rotation;
    node_transform.Decompose(scale, rotation, translation);

    Vec3 node_translation = Vec3(translation.x, translation.y, translation.z);
    Vec3 node_scaling = Vec3(scale.x, scale.y, scale.z);
    Quat node_rotation = Quat(rotation.x, rotation.y, rotation.z, rotation.w);
    
    node->SetTranslationScalingRotation(node_translation, node_scaling, node_rotation);

    //iterate over all meshes in this node
    uint32 meshes_num = node_assimp->mNumMeshes;
    for(uint32 ch_i = 0; ch_i < meshes_num; ch_i ++){
        aiMesh* mesh = scene->mMeshes[node_assimp->mMeshes[ch_i]];
        //Push mesh
        node->mesh_names.push_back(mesh->mName.C_Str());
    }
    //iterate over all children nodes and write their names
    uint32 nodes_num = node_assimp->mNumChildren;
    for(uint32 ch_i = 0; ch_i < nodes_num; ch_i ++){
        aiNode* child = node_assimp->mChildren[ch_i];
        SceneNode* mNode = new SceneNode;
        mNode->SetNodeLabel( child->mName.C_Str());

        processNodeForTree(mNode, child, scene);
        node->children.push_back(mNode);
    }
}

bool VSGEditor::isBoneAvailable(std::string bone_label, const aiScene* scene){
    uint32 mesh_num = scene->mNumMeshes;
    for(uint32 i = 0; i < mesh_num; i ++){
        aiMesh* mesh = scene->mMeshes[i];
        uint32 bones_num = mesh->mNumBones;
        for(uint32 b_i = 0; b_i < bones_num; b_i ++){
            aiBone* bn = mesh->mBones[b_i];
            if(bone_label.compare(bn->mName.C_Str()) == false)
                return true;
        }
    }
    return false;
}
