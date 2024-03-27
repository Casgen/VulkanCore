#pragma once

#include <assimp/scene.h>

#include <string>

#include "Mesh.h"

class Model {

   public:
    Model();
    Model(const std::string& filePath);

    std::vector<Mesh>& GetMeshes() { return meshes_; }
    uint32_t GetMeshletCount() const { return meshlet_count_; }

    void Draw(const vk::CommandBuffer& cmdBuffer, const vk::Pipeline& pipeline,
              const vk::PipelineLayout& pipelineLayout);

   private:
    std::vector<Mesh> meshes_;
    uint32_t meshlet_count_ = 0;

    void ProcessNode(const aiNode* node, const aiScene* scene);
    Mesh ProcessMesh(const aiMesh* mesh, const aiScene* scene);
};
