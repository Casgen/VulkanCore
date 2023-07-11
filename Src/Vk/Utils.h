#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <set>

#include "vulkan/vulkan.hpp"

namespace VkCore
{
    struct SwapChainSupportDetails;

    class Utils
    {
      public:
        /**
         * @brief Creates a new Vulkan Instance
         * @param appName arbitrary application name
         * @param vkApiVersion Defines the Vulkan API version which will be used for creation. Use with
         * VK_API_VERSION_1_X
         * @param enableValidationLayer Enables validation layers for debugging. This way everything that Vulkan does
         * can be logged. @param appVersion arbitrary version of the app.
         * @param engineName arbitrary engine name.
         * @param engineVersion arbitrary engine version.
         */
        static vk::Instance CreateInstance(const std::string &appName, const uint32_t vkApiVersion,
                                           const std::vector<const char *> &instanceExtensions,
                                           const uint32_t appVersion = VK_MAKE_VERSION(0, 0, 0),
                                           const std::string &engineName = "Engine",
                                           const uint32_t engineVersion = VK_MAKE_VERSION(0, 0, 0));

        /**
         * @brief Creates a DebugMessengerCreateInfo object used within the VkInstance object.
         */
        static vk::DebugUtilsMessengerCreateInfoEXT PopulateDebugMessengerCreateInfo();

        /**
         * @brief Acts as a callback function for the validation layers. With this the errors are logged into the
         * console. DO NOT USE IT BY ITSELF.
         * @param messageSeverity A bitfield representing level of severity
         * @param messageType A bitfield representing a type of message
         * @param pCallbackData
         * @param pUserData
         */
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                            VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                            void *pUserData);

        static bool CheckValidationLayerSupport();
        /**
         * @brief Checks the result of a Vulkan operation. The function may abort the program if the result is an error!
         * @param result a VkResult object.
         */
        static void CheckVkResult(vk::Result result);

        /**
         * @brief Checks the result of a Vulkan operation. The function may abort the program if the result is an error!
         * @param result a VkResult object.
         */
        static void CheckVkResult(VkResult result);

        inline static std::set<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
        
    };

} // namespace VkCore
//
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(VkInstance instance,
                                                              const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                                              const VkAllocationCallbacks *pAllocator,
                                                              VkDebugUtilsMessengerEXT *pMessenger);

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger,
                                                           VkAllocationCallbacks const *pAllocator);

