#pragma once

#include <set>
#include <string>
#include <vulkan/vulkan.hpp>

#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_structs.hpp"

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
                       std::set<std::string> requiredExtensions);

        /**
         * @brief Checks if the physical device supports the defined extensions;
         * @param physicalDevice
         */
        bool CheckDeviceExtensionSupport(std::set<std::string> requiredExtensions) const;

        /**
         * @brief Queries the swap chain support details of the physical device;
         */
        SwapChainSupportDetails QuerySwapChainSupport(const vk::SurfaceKHR& surface) const;

        /**
         * @brief Creates a logical device from this physical device.
         * @param deviceCreateInfo
         */
        vk::Device CreateDevice(const vk::DeviceCreateInfo& deviceCreateInfo) const;

        // ------------ GETTERS ------------

        SwapChainSupportDetails GetSwapChainSupportDetails() const;
        QueueFamilyIndices GetQueueFamilyIndices() const;
        vk::PhysicalDevice& GetPhysicalDevice();

      private:
        vk::PhysicalDevice m_PhysicalDevice;

        SwapChainSupportDetails m_SwapChainDetails;
        QueueFamilyIndices m_QueueFamilyIndices;

        /**
         * @brief Enumerates all the queue families and chooses their index. They are
         * used later for submitting commands to those queues.
         * @param surface
         */
        QueueFamilyIndices FindQueueFamilyIndices(const vk::SurfaceKHR& surface);
    };
} // namespace VkCore
