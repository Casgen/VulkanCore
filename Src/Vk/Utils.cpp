#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

#include "../Log/Log.h"
#include "Devices/DeviceManager.h"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_hpp_macros.hpp"
#include "vulkan/vulkan_structs.hpp"
#include "Utils.h"

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace VkCore
{
    vk::Instance Utils::CreateInstance(const std::string& appName, const uint32_t vkApiVersion,
                                       const std::vector<const char*>& instanceExtensions,
                                       const std::vector<const char*>& layerExtensions, const uint32_t appVersion,
                                       const std::string& engineName, const uint32_t engineVersion)
    {

        std::vector<vk::ExtensionProperties> extensionProperties = vk::enumerateInstanceExtensionProperties();

        for (const auto& extension : instanceExtensions)
        {

            for (const auto& prop : extensionProperties)
            {
                if (strcmp(prop.extensionName, extension))
                {
                    break;
                }

                LOGF(Vulkan, Fatal,
                     "The desired extension %s was not found! This could be due to a misspell or an outdated version!",
                     extension)
                throw std::runtime_error(
                    "The desired extension was not found! This could be due to a misspell or an outdated version!");
            }
        }

        vk::ApplicationInfo appInfo = {appName.c_str(), appVersion, engineName.c_str(), engineVersion, vkApiVersion};

        vk::InstanceCreateInfo instanceCreateInfo({}, &appInfo);

        instanceCreateInfo.setPEnabledExtensionNames(instanceExtensions);

#if DEBUG
        vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo = PopulateDebugMessengerCreateInfo();

        if (CheckValidationLayerSupport())
        {
            instanceCreateInfo.setPEnabledLayerNames(m_ValidationLayers);
            instanceCreateInfo.setPNext(&debugCreateInfo);
        }
#endif

        // PFN_vkCmdDrawMeshTasksNV vkCmdDrawMeshTasksNV = vkGetDeviceProcAddr(VkDevice device, const char *pName)

        vk::Instance instance;

        try
        {
            instance = vk::createInstance(instanceCreateInfo);

            vk::DispatchLoaderDynamic dli{};
            dli.init();
            dli.init(instance);
        }
        catch (const vk::SystemError& err)
        {
            std::string errMsg = std::string("Failed to create VkInstance!: ").append(err.what());

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
    vk::Format Utils::FindSupportedFormat(const std::vector<vk::Format>& candidates, const vk::ImageTiling tiling,
                                          vk::FormatFeatureFlags featureFlags)
    {
        for (vk::Format format : candidates)
        {

            vk::FormatProperties props = DeviceManager::GetPhysicalDevice().GetFormatProperties(format);

            if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & featureFlags) == featureFlags)
            {
                return format;
            }
            else if (tiling == vk::ImageTiling::eOptimal &&
                     (props.optimalTilingFeatures & featureFlags) == featureFlags)
            {
                return format;
            }
        }

        const char* errMsg = "Couldn't find supported format!";
        LOG(Vulkan, Error, errMsg)
        throw std::runtime_error(errMsg);
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

        Logger::GetLogger()->Print(category, severity, pCallbackData->pMessage);

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

        createInfo.setPfnUserCallback(&Utils::DebugCallback)
            .setFlags({})
            .setMessageType(messageTypes)
            .setMessageSeverity(severityFlags);

        return createInfo;
    }

    bool Utils::CheckValidationLayerSupport()
    {
        std::vector<vk::LayerProperties> layerProperties = vk::enumerateInstanceLayerProperties();

        for (const auto& layer : m_ValidationLayers)
        {
            bool layerFound = false;

            for (const auto& property : layerProperties)
            {
                if (strcmp(property.layerName, layer))
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
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
