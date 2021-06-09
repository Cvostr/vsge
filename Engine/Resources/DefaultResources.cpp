#include "DefaultResources.hpp"
#include "ResourceCache.hpp"

#include "ResourceTypes/MaterialResource.hpp"
#include "ResourceTypes/MeshResource.hpp"

using namespace VSGE;

static Vertex cube_vertices[] = {
    // back face
    Vertex(Vec3(-1.0f, -1.0f, -1.0f), Vec2(0.0f, 0.0f), Vec3(0.0f,  0.0f, -1.0f)), // bottom-left
    Vertex(Vec3(1.0f,  1.0f, -1.0f), Vec2(1.0f, 1.0f), Vec3(0.0f,  0.0f, -1.0f)), // top-right
    Vertex(Vec3(1.0f, -1.0f, -1.0f), Vec2(1.0f, 0.0f), Vec3(0.0f,  0.0f, -1.0f)), // bottom-right
    Vertex(Vec3(1.0f,  1.0f, -1.0f), Vec2(1.0f, 1.0f), Vec3(0.0f,  0.0f, -1.0f)), // top-right
    Vertex(Vec3(-1.0f, -1.0f, -1.0f), Vec2(0.0f, 0.0f), Vec3(0.0f,  0.0f, -1.0f)), // bottom-left
    Vertex(Vec3(-1.0f,  1.0f, -1.0f), Vec2(0.0f, 1.0f), Vec3(0.0f,  0.0f, -1.0f)), // top-left
    // front face
    Vertex(Vec3(-1.0f, -1.0f,  1.0f), Vec2(0.0f, 0.0f), Vec3(0.0f,  0.0f,  1.0f)), // bottom-left
    Vertex(Vec3(1.0f, -1.0f,  1.0f), Vec2(1.0f, 0.0f), Vec3(0.0f,  0.0f,  1.0f)), // bottom-right
    Vertex(Vec3(1.0f,  1.0f,  1.0f), Vec2(1.0f, 1.0f), Vec3(0.0f,  0.0f,  1.0f)), // top-right
    Vertex(Vec3(1.0f,  1.0f,  1.0f), Vec2(1.0f, 1.0f), Vec3(0.0f,  0.0f,  1.0f)), // top-right
    Vertex(Vec3(-1.0f,  1.0f,  1.0f), Vec2(0.0f, 1.0f), Vec3(0.0f,  0.0f,  1.0f)), // top-left
    Vertex(Vec3(-1.0f, -1.0f,  1.0f), Vec2(0.0f, 0.0f), Vec3(0.0f,  0.0f,  1.0f)), // bottom-left
    // left face
    Vertex(Vec3(-1.0f,  1.0f,  1.0f), Vec2(1.0f, 0.0f), Vec3(-1.0f,  0.0f,  0.0f)), // top-right
    Vertex(Vec3(-1.0f,  1.0f, -1.0f), Vec2(1.0f, 1.0f), Vec3(-1.0f,  0.0f,  0.0f)), // top-left
    Vertex(Vec3(-1.0f, -1.0f, -1.0f), Vec2(0.0f, 1.0f), Vec3(-1.0f,  0.0f,  0.0f)), // bottom-left
    Vertex(Vec3(-1.0f, -1.0f, -1.0f), Vec2(0.0f, 1.0f), Vec3(-1.0f,  0.0f,  0.0f)), // bottom-left
    Vertex(Vec3(-1.0f, -1.0f,  1.0f), Vec2(0.0f, 0.0f), Vec3(-1.0f,  0.0f,  0.0f)), // bottom-right
    Vertex(Vec3(-1.0f,  1.0f,  1.0f), Vec2(1.0f, 0.0f), Vec3(-1.0f,  0.0f,  0.0f)), // top-right
    // right face
    Vertex(Vec3(1.0f,  1.0f,  1.0f), Vec2(1.0f, 0.0f), Vec3(1.0f,  0.0f,  0.0f)), // top-left
    Vertex(Vec3(1.0f, -1.0f, -1.0f), Vec2(0.0f, 1.0f), Vec3(1.0f,  0.0f,  0.0f)), // bottom-right
    Vertex(Vec3(1.0f,  1.0f, -1.0f), Vec2(1.0f, 1.0f), Vec3(1.0f,  0.0f,  0.0f)), // top-right
    Vertex(Vec3(1.0f, -1.0f, -1.0f), Vec2(0.0f, 1.0f), Vec3(1.0f,  0.0f,  0.0f)), // bottom-right
    Vertex(Vec3(1.0f,  1.0f,  1.0f), Vec2(1.0f, 0.0f), Vec3(1.0f,  0.0f,  0.0f)), // top-left
    Vertex(Vec3(1.0f, -1.0f,  1.0f), Vec2(0.0f, 0.0f), Vec3(1.0f,  0.0f,  0.0f)), // bottom-left
    // bottom face
    Vertex(Vec3(-1.0f, -1.0f, -1.0f), Vec2(0.0f, 1.0f), Vec3(0.0f, -1.0f,  0.0f)), // top-right
    Vertex(Vec3(1.0f, -1.0f, -1.0f), Vec2(1.0f, 1.0f), Vec3(0.0f, -1.0f,  0.0f)), // top-left
    Vertex(Vec3(1.0f, -1.0f,  1.0f), Vec2(1.0f, 0.0f), Vec3(0.0f, -1.0f,  0.0f)), // bottom-left
    Vertex(Vec3(1.0f, -1.0f,  1.0f), Vec2(1.0f, 0.0f), Vec3(0.0f, -1.0f,  0.0f)), // bottom-left
    Vertex(Vec3(-1.0f, -1.0f,  1.0f), Vec2(0.0f, 0.0f), Vec3(0.0f, -1.0f,  0.0f)), // bottom-right
    Vertex(Vec3(-1.0f, -1.0f, -1.0f), Vec2(0.0f, 1.0f), Vec3(0.0f, -1.0f,  0.0f)), // top-right
    // top face
    Vertex(Vec3(-1.0f,  1.0f, -1.0f), Vec2(0.0f, 1.0f), Vec3(0.0f,  1.0f,  0.0f)), // top-left
    Vertex(Vec3(1.0f,  1.0f , 1.0f), Vec2(1.0f, 0.0f), Vec3(0.0f,  1.0f,  0.0f)), // bottom-right
    Vertex(Vec3(1.0f,  1.0f, -1.0f), Vec2(1.0f, 1.0f), Vec3(0.0f,  1.0f,  0.0f)), // top-right
    Vertex(Vec3(1.0f,  1.0f,  1.0f), Vec2(1.0f, 0.0f), Vec3(0.0f,  1.0f,  0.0f)), // bottom-right
    Vertex(Vec3(-1.0f,  1.0f, -1.0f), Vec2(0.0f, 1.0f), Vec3(0.0f,  1.0f,  0.0f)), // top-left
    Vertex(Vec3(-1.0f,  1.0f,  1.0f), Vec2(0.0f, 0.0f), Vec3(0.0f,  1.0f,  0.0f))  // bottom-left
};


void VSGE::AddDefaultMaterial() {
	MaterialResource* default_mat = new MaterialResource;
	default_mat->SetName("Default Material");
    default_mat->SetState(RESOURCE_STATE_READY);
	ResourceCache::Get()->PushResource(default_mat);
}

void VSGE::AddDefaultMeshes() {
    VertexSkinningData* verts = new VertexSkinningData[36];

	MeshResource* cube_mesh = new MeshResource;
	cube_mesh->SetName("Cube");
    cube_mesh->GetMesh()->SetVertexBuffer(cube_vertices, 36);
    cube_mesh->GetMesh()->AddVertexBuffer(verts);
    cube_mesh->GetMesh()->Create();
    cube_mesh->SetState(RESOURCE_STATE_READY);
	ResourceCache::Get()->PushResource(cube_mesh);
    delete[] verts;

    //MeshResource* sphere_mesh = new MeshResource;
    //sphere_mesh->SetName("Sphere");
    //cube_mesh->GetMesh()->
    //ResourceCache::Get()->PushResource(sphere_mesh);
}