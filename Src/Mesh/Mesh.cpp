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
    : indices_(indices), vertices_(vertices) {

    vertex_buffer_ = VkCore::Buffer(vk::BufferUsageFlagBits::eStorageBuffer);
    vertex_buffer_.InitializeOnGpu(vertices.data(), vertices.size() * sizeof(MeshVertex));

    const std::vector<Meshlet> meshlets = MeshletGeneration::MeshletizeUnoptimized(
        Constants::MAX_MESHLET_VERTICES, Constants::MAX_MESHLET_INDICES, indices, vertices.size());

    meshlet_count_ = meshlets.size();

    meshlet_buffer_ = VkCore::Buffer(vk::BufferUsageFlagBits::eStorageBuffer);
    meshlet_buffer_.InitializeOnGpu(meshlets.data(), meshlets.size() * sizeof(Meshlet));

    VkCore::DescriptorBuilder descBuilder = VkCore::DescriptorBuilder(VkCore::DeviceManager::GetDevice());

    bool success =
        descBuilder.BindBuffer(0, vertex_buffer_, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eMeshNV)
            .BindBuffer(1, meshlet_buffer_, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eMeshNV)
            .Build(descriptor_set_, descriptor_set_layout_);

    if (!success) {
        LOG(Vulkan, Fatal, "Failed to build a descriptor set for a mesh!")
        throw std::runtime_error("Failed to build a descriptor set for a mesh!");
    }
}
