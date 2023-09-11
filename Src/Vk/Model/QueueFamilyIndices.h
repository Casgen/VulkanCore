#pragma once

#include <cstdint>
#include <optional>
#include <vector>

namespace VkCore
{

    struct QueueFamilyIndices
    {

        std::optional<uint32_t> m_GraphicsFamily, m_PresentFamily, m_ComputeFamily, m_TransferFamily;

        bool IsComplete() const
        {
            return m_GraphicsFamily.has_value() && m_PresentFamily.has_value();
        }

        bool HasCompute() const
        {
            return m_ComputeFamily.has_value();
        }

        bool HasTransfer() const
        {
            return m_ComputeFamily.has_value();
        }
    };

} // namespace VkCore
