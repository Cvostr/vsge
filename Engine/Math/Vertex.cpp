#include "Vertex.hpp"

void ProcessTangentSpace(Vertex* vert_array, uint32* indices_array, uint32 indices_num) {
    for (int ind_i = 0; ind_i < indices_num; ind_i += 3) {
        Vertex v1 = vert_array[indices_array[ind_i]];
        Vertex v2 = vert_array[indices_array[ind_i + 1]];
        Vertex v3 = vert_array[indices_array[ind_i + 2]];

        Vec3 edge1 = v2.pos - v1.pos;
        Vec3 edge2 = v3.pos - v1.pos;
        Vec2 deltaUV1 = v2.uv - v1.uv;
        Vec2 deltaUV2 = v3.uv - v1.uv;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        Vec3 tangent, bitangent;
        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent = tangent.GetNormalized();

        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent = bitangent.GetNormalized();
        for (int i = 0; i < 3; i++) {
            vert_array[indices_array[ind_i + i]].tangent = tangent;
            vert_array[indices_array[ind_i + i]].bitangent = bitangent;
        }
    }
}

void ProcessTangentSpace(Vertex* vert_array, uint32 vertices_num) {
    for (int v_i = 0; v_i < vertices_num; v_i += 3) {
        Vertex v1 = vert_array[v_i];
        Vertex v2 = vert_array[v_i + 1];
        Vertex v3 = vert_array[v_i + 2];

        Vec3 edge1 = v2.pos - v1.pos;
        Vec3 edge2 = v3.pos - v1.pos;
        Vec2 deltaUV1 = v2.uv - v1.uv;
        Vec2 deltaUV2 = v3.uv - v1.uv;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        Vec3 tangent, bitangent;
        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent.Normalize();

        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent.Normalize();

        tangent.z *= -1;
        bitangent.z *= -1;

        tangent.x *= -1;
        bitangent.x *= -1;

        for (int i = v_i; i < v_i + 3; i++) {
            vert_array[i].tangent = tangent;
            vert_array[i].bitangent = bitangent;
        }
    }
}