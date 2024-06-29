#include "Model.h"

#include <cstddef>
#include <iostream>
#include <stdexcept>

#include "../Log/Log.h"
#include "MeshVertex.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "vulkan/vulkan.hpp"

Model::Model()
{
}

Model::Model(const std::string& filePath)
{

    std::cout << "Offset of position: " << offsetof(MeshVertex, Position) << std::endl;
    std::cout << "Offset of normal: " << offsetof(MeshVertex, Normal) << std::endl;
    std::cout << "Offset of tangent: " << offsetof(MeshVertex, Tangent) << std::endl;
    std::cout << "Offset of bitangent: " << offsetof(MeshVertex, BiTangent) << std::endl;
    std::cout << "Offset of texCoords: " << offsetof(MeshVertex, TexCoords) << std::endl;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath.data(), aiProcess_Triangulate | aiProcess_GenNormals);

    if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr)
    {
        LOGF(Assimp, Fatal, "Failed to import a scene! %s\n", importer.GetErrorString())
        throw std::runtime_error("Failed to import a scene! Filepath: " + filePath);
    }

    ProcessNode(scene->mRootNode, scene);
}

void Model::Draw(const vk::CommandBuffer& cmdBuffer, const vk::Pipeline& pipeline,
                 const vk::PipelineLayout& pipelineLayout)
{
}

void Model::ProcessNode(const aiNode* node, const aiScene* scene)
{

    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
        Mesh mesh = ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene);
        m_MeshletCount += mesh.GetMeshletCount();
        m_Meshes.emplace_back(std::move(mesh));
    }

    for (uint32_t i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

Mesh Model::ProcessMesh(const aiMesh* mesh, const aiScene* scene)
{

    if (mesh == nullptr)
    {
        LOG(Assimp, Fatal, "Failed to process a mesh! aiMesh is null!")
        throw std::runtime_error("Failed to process a mesh! aiMesh is null!");
    }

    std::vector<MeshVertex> meshVertices{};
    meshVertices.reserve(mesh->mNumVertices);

    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
        glm::vec3 position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
        glm::vec3 normals = glm::vec3(0.f);

        if (mesh->HasNormals())
        {
            normals = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
        }

        glm::vec2 textureCoord;
        glm::vec3 tangent;
        glm::vec3 biTangent;

        if (mesh->mTextureCoords[0] != nullptr)
        {
            textureCoord = {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y,
            };

            if (mesh->HasTangentsAndBitangents())
            {
                tangent = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
                biTangent = {mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
            }
        }

        meshVertices.emplace_back(MeshVertex{position, normals, tangent, biTangent, textureCoord});
    }

    std::vector<uint32_t> indices;

    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {

        aiFace& face = mesh->mFaces[i];

        for (uint32_t j = 0; j < face.mNumIndices; j++)
        {
            indices.emplace_back(face.mIndices[j]);
        }
    }

    return Mesh(indices, meshVertices);
}
