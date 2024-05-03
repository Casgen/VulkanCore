#include "Mesh.h"

#include <stdexcept>

#include "../Constants.h"
#include "../Log/Log.h"
#include "../Vk/Buffers/Buffer.h"
#include "../Vk/Descriptors/DescriptorBuilder.h"
#include "../Vk/Devices/DeviceManager.h"
#include "Meshlet.h"
#include "MeshletGeneration.h"
#include "vulkan/vulkan_enums.hpp"

Mesh::Mesh(const std::vector<uint32_t>& indices, const std::vector<MeshVertex>& vertices)
    : m_Indices(indices), m_Vertices(vertices)
{

    m_VertexBuffer = VkCore::Buffer(vk::BufferUsageFlagBits::eStorageBuffer);
    m_VertexBuffer.InitializeOnGpu(vertices.data(), vertices.size() * sizeof(MeshVertex));

    const std::vector<Meshlet> meshlets = MeshletGeneration::MeshletizeUnoptimized(
        Constants::MAX_MESHLET_VERTICES, Constants::MAX_MESHLET_INDICES, indices, vertices.size());

    m_MeshletCount = meshlets.size();

    m_MeshletBuffer = VkCore::Buffer(vk::BufferUsageFlagBits::eStorageBuffer);
    m_MeshletBuffer.InitializeOnGpu(meshlets.data(), meshlets.size() * sizeof(Meshlet));

    VkCore::DescriptorBuilder descBuilder = VkCore::DescriptorBuilder(VkCore::DeviceManager::GetDevice());

    bool success =
        descBuilder.BindBuffer(0, m_VertexBuffer, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eMeshNV)
            .BindBuffer(1, m_MeshletBuffer, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eMeshNV)
            .Build(m_DescriptorSet, m_DescriptorSetLayout);

    if (!success)
    {
        LOG(Vulkan, Fatal, "Failed to build a descriptor set for a mesh!")
        throw std::runtime_error("Failed to build a descriptor set for a mesh!");
    }
}
