#include "Utils.h"


namespace VkCore
{
    vk::Instance CreateInstance(const std::string &appName, const uint32_t vkApiVersion,
                                const bool enableValidationLayer, const uint32_t appVersion,
                                const std::string &engineName, const uint32_t engineVersion)
    {
        vk::ApplicationInfo appInfo = {appName.c_str(), appVersion, engineName.c_str(), engineVersion, vkApiVersion};

        vk::InstanceCreateInfo InstanceCreateInfo({}, &appInfo);

        // return vk::createInstance(InstanceCreateInfo);
    }
} // namespace VkCore
