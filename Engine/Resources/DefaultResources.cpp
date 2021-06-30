#include "DefaultResources.hpp"
#include "ResourceCache.hpp"
#include <Math/MathBase.hpp>
#include "ResourceTypes/MaterialResource.hpp"
#include "ResourceTypes/MeshResource.hpp"

using namespace VSGE;

#define SPHERE_SECTORS 36
#define SPHERE_STACKS 18

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


    std::vector<Vertex> sphere_v;
    std::vector<uint32> sphere_indices;
    float radius = 1.f;
    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    float s, t;                                     // vertex texCoord

    float sectorStep = 2 * PI_FLOAT / SPHERE_SECTORS;
    float stackStep = PI_FLOAT / SPHERE_STACKS;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= SPHERE_STACKS; ++i)
    {
        stackAngle = PI_FLOAT / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= SPHERE_SECTORS; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi
            Vertex v;

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            v.pos = Vec3(x, y, z);

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            v.normal = Vec3(nx, ny, nz);

            // vertex tex coord (s, t) range between [0, 1]
            s = static_cast<float>(j) / SPHERE_SECTORS;
            t = static_cast<float>(i) / SPHERE_STACKS;
            v.uv = Vec2(s, t);
            sphere_v.push_back(v);
        }
    }
    unsigned int k1, k2;
    for (unsigned int i = 0; i < SPHERE_STACKS; ++i)
    {
        k1 = i * (SPHERE_SECTORS + 1);     // beginning of current stack
        k2 = k1 + SPHERE_SECTORS + 1;      // beginning of next stack

        for (int j = 0; j < SPHERE_SECTORS; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0)
            {
                sphere_indices.push_back(k1);
                sphere_indices.push_back(k2);
                sphere_indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if (i != (SPHERE_STACKS - 1))
            {
                sphere_indices.push_back(k1 + 1);
                sphere_indices.push_back(k2);
                sphere_indices.push_back(k2 + 1);
            }
        }
    }

    verts = new VertexSkinningData[sphere_v.size()];

    MeshResource* sphere_mesh = new MeshResource;
    sphere_mesh->SetName("Sphere");
    sphere_mesh->GetMesh()->SetVertexBuffer(sphere_v.data(), sphere_v.size());
    sphere_mesh->GetMesh()->SetIndexBuffer(sphere_indices.data(), sphere_indices.size());
    sphere_mesh->GetMesh()->AddVertexBuffer(verts);
    sphere_mesh->GetMesh()->Create();
    sphere_mesh->SetState(RESOURCE_STATE_READY);
    ResourceCache::Get()->PushResource(sphere_mesh);
    delete[] verts;
}