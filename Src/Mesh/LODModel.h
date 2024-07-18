#pragma once

#include <assimp/scene.h>
#include <string>

#include <vector>
#include "Mesh/LODMesh.h"
#include "MeshVertex.h"
#include "vulkan/vulkan.hpp"

struct LODData
{

    std::vector<uint32_t> indices;
    std::vector<MeshVertex> vertices;

    // LODData(LODData&& other) {
    // 	if (this != &other) {
    // 		indices = std::move(other.indices);
    // 		vertices = std::move(other.vertices);
    // 	}
    // }
};

class LODModel
{

  public:
    LODModel() = default;
    LODModel(const std::string& filePath);

    size_t GetMeshCount()
    {
        return m_Meshes.size();
    }

    LODMesh& GetMesh(const size_t index);
    vk::DescriptorSetLayout GetMeshSetLayout(const size_t index);
    vk::DescriptorSet GetMeshSet(const size_t index);

    void Destroy();

  private:
    std::vector<std::vector<LODData>> m_LodData = {};
    std::vector<LODMesh> m_Meshes = {};

    void ProcessNode(const aiNode* node, const aiScene* scene, const uint32_t lodDataIndex);
    LODData ProcessMesh(const aiMesh* mesh, const aiScene* scene);
};
