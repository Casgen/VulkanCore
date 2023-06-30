#pragma once

#include "vulkan/vk_platform.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <memory>
#include <vulkan/vulkan.hpp>

namespace VkCore
{
    /**
     * @brief Creates a new Vulkan Instance
     * @param appName - arbitrary application name
     * @param vkApiVersion - Defines the Vulkan API version which will be used for creation. USW WITH VK_API_VERSION_1_X
     * @param enableValidationLayer - Enables validation layers for debugging. This way everything that Vulkan does can
     * be logged.
     * @param appVersion - arbitrary version of the app.
     * @param engineName - arbitrary engine name.
     * @param engineVersion - arbitrary engine version.
     */
    vk::Instance CreateInstance(const std::string &appName, const uint32_t vkApiVersion,
                                const std::vector<std::string> &instanceExtensions, const uint32_t appVersion = 0,
                                const std::string &engineName = "Engine", const uint32_t engineVersion = 0);

    /**
     * @brief Creates a DebugMessengerCreateInfo object used within the VkInstance object.
     */
    vk::DebugUtilsMessengerCreateInfoEXT PopulateDebugMessengerCreateInfo();

    /**
     * @brief Acts as a callback function for the validation layers. With this the errors are logged into the console.
     */
    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                 VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                 const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                 void *pUserData);

    /**
     * @brief Checks the result of a Vulkan operation. The function may abort the program if the result is an error!
     * @param result a VkResult object.
     */
    void CheckVkResult(VkResult result);
} // namespace VkCore
