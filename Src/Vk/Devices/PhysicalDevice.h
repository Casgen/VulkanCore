#pragma once

#include "vulkan/vulkan.hpp"
#include "../QueueFamilyIndices.h"
#include "../SwapChainSupportDetails.h"

namespace VkCore
{
    class PhysicalDevice
    {

      public:
        PhysicalDevice() = default;

        /**
         * @brief Enumerates physicalDevices and picks automatically the most suitable one.
         * @param instance
         * @param A set of strings defining required physical device extensions. They should be defined with the Vulkan
         * preprocessor macros. Not manually!
         */
        PhysicalDevice(const vk::Instance& instance, const vk::SurfaceKHR& surface,
                       const std::vector<const char*>& requiredExtensions);

        /**
         * @brief Creates a logical device from this physical device.
         * @param deviceCreateInfo
         */
        vk::Device CreateDevice(const vk::DeviceCreateInfo& deviceCreateInfo) const;

        /**
         * @brief Checks if the physical device supports the defined extensions;
         * @param physicalDevice
         */
        static bool CheckDeviceExtensionSupport(const vk::PhysicalDevice& physicalDevice, std::vector<const char*> requiredExtensions);

        /**
         * @brief Queries the swap chain support details of the physical device;
         */
        static SwapChainSupportDetails QuerySwapChainSupport(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface);

        /**
         * @brief Enumerates all the queue families and chooses their index. They are
         * used later for submitting commands to those queues.
         * @param surface
         */
        static QueueFamilyIndices FindQueueFamilyIndices(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface);


        // ------------ GETTERS ------------

        SwapChainSupportDetails GetSwapChainSupportDetails() const;
        QueueFamilyIndices GetQueueFamilyIndices() const;
        vk::PhysicalDeviceProperties GetProperties() const;
        vk::PhysicalDeviceMemoryProperties GetMemoryProperties() const;

        vk::PhysicalDevice& operator*();

      private:
        vk::PhysicalDevice m_PhysicalDevice;

        SwapChainSupportDetails m_SwapChainDetails;
        QueueFamilyIndices m_QueueFamilyIndices;

    };
} // namespace VkCore
