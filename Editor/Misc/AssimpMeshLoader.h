#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Math/Mat4.hpp>
#include <Math/Quaternion.hpp>
#include <Graphics/Bone.hpp>
#include <Graphics/Animation.hpp>
#include <Resources/ModelParser.hpp>
#include <string>

namespace VSGEditor {

void cmat(aiMatrix4x4 matin, Mat4* matout);

void processNodeForTree(VSGE::SceneNode* node, aiNode* node_assimp, const aiScene* scene);
void loadNodeTree(std::string file_path, VSGE::SceneNode* node);

bool getSizes(std::string file_path, unsigned int* meshes, unsigned int* anims, unsigned int* textures = nullptr, unsigned int* materials = nullptr);

void loadMesh(std::string file_path, VSGE::MeshContainer* mesh_ptr, int index);

void loadAnimation(std::string file_path, VSGE::Animation* anim, int index);

void loadMaterial(std::string file_path, aiMaterial* material, int index);

void processMesh(aiMesh* mesh, VSGE::MeshContainer* mesh_ptr);

bool isBoneAvailable(std::string bone_label, const aiScene* scene);
}