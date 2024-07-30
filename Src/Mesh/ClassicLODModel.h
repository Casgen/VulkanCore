#pragma once

#include <assimp/scene.h>
#include <string>

#include <vector>
#include "Mesh/ClassicLODMesh.h"
#include "Mesh/LODMesh.h"
#include "MeshVertex.h"

struct LODData
{
    std::vector<uint32_t> indices;
    std::vector<Vertex> vertices;
};

class ClassicLODModel
{

  public:
    ClassicLODModel() = default;
    ClassicLODModel(const std::string& filePath);

    size_t GetMeshCount()
    {
        return m_Meshes.size();
    }

    ClassicLODMesh& GetMesh(const size_t index);

    void Destroy();

  private:
    std::vector<std::vector<LODData>> m_LodData = {};
    std::vector<ClassicLODMesh> m_Meshes = {};

    void ProcessNode(const aiNode* node, const aiScene* scene, const uint32_t lodDataIndex);
    LODData ProcessMesh(const aiMesh* mesh, const aiScene* scene);
};
