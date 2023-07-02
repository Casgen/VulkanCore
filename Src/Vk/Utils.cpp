#include "Utils.h"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_to_string.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace VkCore
{
        vk::Instance Utils::CreateInstance(const std::string &appName, const uint32_t vkApiVersion,
                                    const std::vector<std::string> &instanceExtensions, const uint32_t appVersion,
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

    VKAPI_ATTR VkBool32 VKAPI_CALL Utils::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                 VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                 const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                 void *pUserData) {
        return false;
    }

    vk::DebugUtilsMessengerCreateInfoEXT Utils::PopulateDebugMessengerCreateInfo()
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
            .setPfnUserCallback(&Utils::DebugCallback)
            .setFlags({})
            .setMessageType(messageTypes)
            .setMessageSeverity(severityFlags);

        return createInfo;
    }

    void Utils::CheckVkResult(VkResult result)
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

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(VkInstance instance,
                                                              const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                                              const VkAllocationCallbacks *pAllocator,
                                                              VkDebugUtilsMessengerEXT *pMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    if (func != nullptr)
        return func(instance, pCreateInfo, pAllocator, pMessenger);

    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger,
                                                           VkAllocationCallbacks const *pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

    if (func != nullptr)
        return func(instance, messenger, pAllocator);
}
