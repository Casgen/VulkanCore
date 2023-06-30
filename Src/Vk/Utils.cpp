#include "Utils.h"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_enums.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace VkCore
{
    vk::Instance CreateInstance(const std::string &appName, const uint32_t vkApiVersion,
                                const std::vector<std::string> instanceExtensions, const uint32_t appVersion,
                                const std::string &engineName, const uint32_t engineVersion)
    {
        vk::ApplicationInfo appInfo = {appName.c_str(), appVersion, engineName.c_str(), engineVersion, vkApiVersion};

        vk::InstanceCreateInfo InstanceCreateInfo({}, &appInfo);

        if (!instanceExtensions.empty())
        {

            // Need to convert it to normal C-like strings.
            std::vector<const char *> cExtensions;

            for (const std::string &extension : instanceExtensions)
            {
                cExtensions.push_back(extension.c_str());
            }

            InstanceCreateInfo.enabledExtensionCount = instanceExtensions.size();
            InstanceCreateInfo.ppEnabledExtensionNames = cExtensions.data();
        }

        vk::Instance instance;

        try
        {
            instance = vk::createInstance(InstanceCreateInfo);
        }
        catch (const vk::SystemError &err)
        {
            std::string errMsg = std::string("Failed to create VkInstance!: ").append(errMsg);

            std::cerr << errMsg << std::endl;
            std::runtime_error(errMsg.c_str());
        }

        return instance;
    }

    vk::DebugUtilsMessengerCreateInfoEXT PopulateDebugMessengerCreateInfo()
    {

        vk::DebugUtilsMessageSeverityFlagsEXT severityFlags;

        severityFlags =
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

        vk::DebugUtilsMessageTypeFlagsEXT messageTypes = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral;

        messageTypes =
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;

        vk::DebugUtilsMessengerCreateInfoEXT createInfo;

        createInfo.setPNext(nullptr)
            .setPfnUserCallback(&DebugCallback)
            .setFlags({})
            .setMessageType(messageTypes)
            .setMessageSeverity(severityFlags);

        return createInfo;
    }

    void CheckVkResult(VkResult result)
    {
        if (result != 0)
        {
            std::cerr << " Vulkan error: " << vk::to_string(static_cast<vk::Result>(result));
            if (result < 0)
            {
                abort();
            }
        }
    }

} // namespace VkCore
