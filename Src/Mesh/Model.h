#pragma once

#include <assimp/scene.h>

#include <string>

#include "Mesh.h"

class Model
{

  public:
    Model();
    Model(const std::string& filePath);

    std::vector<Mesh>& GetMeshes()
    {
        return m_Meshes;
    }
    uint32_t GetMeshletCount() const
    {
        return m_MeshletCount;
    }

    void Draw(const vk::CommandBuffer& cmdBuffer, const vk::Pipeline& pipeline,
              const vk::PipelineLayout& pipelineLayout);

    void Destroy()
    {
        for (Mesh& mesh : m_Meshes)
        {
            mesh.Destroy();
        }
    }

  private:
    std::vector<Mesh> m_Meshes;
    uint32_t m_MeshletCount = 0;

    void ProcessNode(const aiNode* node, const aiScene* scene);
    Mesh ProcessMesh(const aiMesh* mesh, const aiScene* scene);
};
