
#include <cstdint>
#include <memory>
#include <vulkan/vulkan.hpp>

namespace VkCore
{
    /**
     * @brief Creates a new Vulkan Instance
     * @param appName - arbitrary application name
     * @param vkApiVersion - Defines the Vulkan API version which will be used for creation. USW WITH VK_API_VERSION_1_X
     * @param enableValidationLayer - Enables validation layers for debugging. This way everything that Vulkan does can be logged.
     * @param appVersion - arbitrary version of the app.
     * @param engineName - arbitrary engine name.
     * @param engineVersion - arbitrary engine version.
     */
    vk::Instance CreateInstance(const std::string &appName, const uint32_t vkApiVersion,
                                const bool enableValidationLayer = false, const uint32_t appVersion = 0,
                                const std::string &engineName = "Engine", const uint32_t engineVersion = 0);

} // namespace VkCore
