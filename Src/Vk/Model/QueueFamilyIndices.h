
#include <cstdint>
#include <optional>

namespace VkCore {

    struct QueueFamilyIndices {

    std::optional<uint32_t> m_GraphicsFamily, m_PresentFamily, m_ComputeFamily, m_TransferFamily = 0;
    
    bool isComplete() { return m_GraphicsFamily.has_value() && m_PresentFamily; }

    };
}
