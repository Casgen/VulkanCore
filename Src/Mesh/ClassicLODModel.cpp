
#include "ClassicLODModel.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <filesystem>

#include "../Log/Log.h"
#include "MeshVertex.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

namespace fs = std::filesystem;

ClassicLODModel::ClassicLODModel(const std::string& filePath)
{

    fs::path modelPath(filePath);

    fs::path parentPath = modelPath.parent_path();

    ASSERTF(std::filesystem::is_directory(parentPath),
            "Imported model doesn't have a parent directory! Can not load LODs!: %s", filePath.data())

    std::string modelStem = modelPath.stem().string();

    char* temp = std::strtok(modelStem.data(), "_");      // get only the model name ("lucy_lod0" -> "lucy")
    int modelNameSize = std::snprintf(nullptr, 0, "%s_lod", temp); // Format it to not have a LOD count;

    ASSERT(modelNameSize > 0, "Failed to format the modelname!")

    char* modelName = new char[modelNameSize + 1];
    std::sprintf(modelName, "%s_lod", temp); // Format it to not have a LOD count;

    std::vector<fs::path> lodModelPaths = {};

    for (const auto& entry : std::filesystem::directory_iterator(parentPath))
    {

        // should obtain the entry's filename "lucy_lod1"
        std::string candidateName = entry.path().stem().string();
        std::string ext = entry.path().extension().string();

        // Checks whether the file contains the keyword "lod".
        if (candidateName.find(modelName) != std::string::npos && ext.compare(".mtl") != 0)
        {
            lodModelPaths.emplace_back(entry.path());
        }
    }

    ASSERT(lodModelPaths.size() <= 8, "There are more LODs than supported!");

    std::sort(lodModelPaths.begin(), lodModelPaths.end());

	m_LodData.resize(lodModelPaths.size());

    for (uint8_t i = 0; i < lodModelPaths.size(); i++)
    {
        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(
            lodModelPaths[i].string(), aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices);

        ASSERTF(scene != nullptr && !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) && scene->mRootNode != nullptr,
                "Failed to import a scene! %s", importer.GetErrorString())

        ProcessNode(scene->mRootNode, scene, i);
    }


    ASSERT(m_LodData.size() > 0 && m_LodData.size() == lodModelPaths.size(), "There are not that many or no lod scenes to process!");

    // Construct the LOD meshes
    const uint32_t expectedSize = m_LodData[0].size();

    for (uint32_t i = 1; i < m_LodData.size(); i++)
    {
        ASSERT(expectedSize == m_LodData[i].size(), "The amount of LOD Data structs don't match ");
    }

    for (uint32_t i = 0; i < expectedSize; i++)
    {

        std::vector<LODData> meshLods;

        for (uint32_t meshIndex = 0; meshIndex < m_LodData.size(); meshIndex++)
        {
            meshLods.emplace_back(std::move(m_LodData[meshIndex][i]));
        }

        m_Meshes.emplace_back(meshLods);
    }
}

void ClassicLODModel::ProcessNode(const aiNode* node, const aiScene* scene, const uint32_t lodDataIndex)
{

    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
        m_LodData[lodDataIndex].emplace_back(ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene));
    }

    for (uint32_t i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, lodDataIndex);
    }
}

void ClassicLODModel::Destroy()
{
    for (uint32_t i = 0; i < m_Meshes.size(); i++)
    {
        m_Meshes[i].Destroy();
    }
}

ClassicLODMesh& ClassicLODModel::GetMesh(const size_t index)
{
    return m_Meshes.at(index);
}

LODData ClassicLODModel::ProcessMesh(const aiMesh* mesh, const aiScene* scene)
{

    if (mesh == nullptr)
    {
        LOG(Assimp, Fatal, "Failed to process a mesh! aiMesh is null!")
        throw std::runtime_error("Failed to process a mesh! aiMesh is null!");
    }

    std::vector<Vertex> meshVertices{};
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

        meshVertices.emplace_back(Vertex{position, normals, tangent, biTangent, textureCoord});
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

    return {.indices = indices, .vertices = meshVertices};
}
