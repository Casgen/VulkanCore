#include <algorithm>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

#include "../Log/Log.h"
#include "Utils.h"
#include "Model/SwapChainSupportDetails.h"

namespace VkCore
{
    vk::Instance Utils::CreateInstance(const std::string& appName, const uint32_t vkApiVersion,
                                       const std::vector<const char*>& instanceExtensions, const uint32_t appVersion,
                                       const std::string& engineName, const uint32_t engineVersion)
    {
        vk::ApplicationInfo appInfo = {appName.c_str(), appVersion, engineName.c_str(), engineVersion, vkApiVersion};

        vk::InstanceCreateInfo instanceCreateInfo({}, &appInfo);

        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();

        vk::Instance instance;

        try
        {
            instance = vk::createInstance(instanceCreateInfo);
        }
        catch (const vk::SystemError& err)
        {
            std::string errMsg = std::string("Failed to create VkInstance!: ").append(errMsg);

            std::cerr << errMsg << std::endl;
            exit(-1);
        }
        catch (std::exception& err)
        {
            std::cout << "std::exception: " << err.what() << std::endl;
            LOG(Exception, Fatal, "Std::exception");
            exit(-1);
        }
        catch (...)
        {
            std::cout << "unknown error\n";
            exit(-1);
        }

        return instance;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL Utils::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void* pUserData)
    {
        ECategory category;

        switch (static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(messageType))
        {
        case vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral:
            category = ECategory::General;
        case vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation:
            category = ECategory::Validation;
        case vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance:
            category = ECategory::Performance;
        case vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding:
            category = ECategory::DeviceAddressBinding;
        }

        const ESeverity severity = static_cast<ESeverity>(messageSeverity);

        Logger::Print(category, severity, pCallbackData->pMessage);

        return VK_FALSE;
    }

    vk::DebugUtilsMessengerCreateInfoEXT Utils::PopulateDebugMessengerCreateInfo()
    {

        vk::DebugUtilsMessageSeverityFlagsEXT severityFlags;

        severityFlags =
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

        vk::DebugUtilsMessageTypeFlagsEXT messageTypes = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                         vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                                                         vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding |
                                                         vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;

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

    bool Utils::CheckValidationLayerSupport()
    {
        std::vector<vk::LayerProperties> layerProperties = vk::enumerateInstanceLayerProperties();

        for (const vk::LayerProperties& property : layerProperties)
        {

            for (const auto layer : m_ValidationLayers)
            {
                if (strcmp(property.layerName, layer))
                {
                    continue;
                }
            }

            return false;
        }

        return true;
    }

    void Utils::CheckVkResult(vk::Result result)
    {
        if (result != vk::Result::eSuccess)
        {
            LOG(Vulkan, Error, vk::to_string(result).c_str())
            abort();
        }
    }

    void Utils::CheckVkResult(VkResult result)
    {
        if (result != 0)
        {
            LOG(Vulkan, Error, vk::to_string(static_cast<vk::Result>(result)).c_str())

            if (result < 0)
                abort();
        }
    }

} // namespace VkCore

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(VkInstance instance,
                                                              const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                              const VkAllocationCallbacks* pAllocator,
                                                              VkDebugUtilsMessengerEXT* pMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    if (func != nullptr)
        return func(instance, pCreateInfo, pAllocator, pMessenger);

    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger,
                                                           VkAllocationCallbacks const* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

    if (func != nullptr)
        return func(instance, messenger, pAllocator);
}
