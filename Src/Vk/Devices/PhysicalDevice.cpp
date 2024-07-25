#include "PhysicalDevice.h"
#include "../../Log/Log.h"
#include "../Utils.h"
#include "../SwapChainSupportDetails.h"
#include "vulkan/vulkan_structs.hpp"
#include <algorithm>
#include <stdexcept>

namespace VkCore
{
    PhysicalDevice::PhysicalDevice(const vk::Instance& instance, const vk::SurfaceKHR& surface,
                                   const std::vector<const char*>& requiredExtensions)
    {

        std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();

        if (physicalDevices.size() == 0)
        {
            LOG(Vulkan, Fatal, "None of the physical devices support Vulkan!");
        }

#if DEBUG
        for (const char* ext : requiredExtensions)
        {
            LOGF(Vulkan, Verbose, "Required Extension - %s", ext)
        }
#endif

        for (const auto& device : physicalDevices)
        {
            vk::PhysicalDeviceProperties deviceProperties = device.getProperties();

            LOGF(Vulkan, Verbose, "Testing device %s", deviceProperties.deviceName.data())

            // Define it beforehand, so the next operations can be done.
            QueueFamilyIndices indices = FindQueueFamilyIndices(device, surface);

            bool extensionsSupported = CheckDeviceExtensionSupport(device, requiredExtensions);

            bool swapChainAdequate = false;
            SwapChainSupportDetails swapChainSupport;

            if (!extensionsSupported)
            {
                LOGF(Vulkan, Verbose, "Device %s does not support the extensions!", deviceProperties.deviceName.data())
                continue;
            }

            LOGF(Vulkan, Verbose, "Device %s does support the extensions!", deviceProperties.deviceName.data())

            swapChainSupport = QuerySwapChainSupport(device, surface);
            swapChainAdequate = !swapChainSupport.m_SurfaceFormats.empty() && !swapChainSupport.m_PresentModes.empty();

            if (indices.IsComplete() && swapChainAdequate)
            {

                LOGF(Vulkan, Info, "Suitable physical device has been found. Using %s",
                     deviceProperties.deviceName.data())

                m_QueueFamilyIndices = indices;
                m_PhysicalDevice = device;
                m_SwapChainDetails = swapChainSupport;
                return;
            }

            LOGF(Vulkan, Verbose,
                 "Device %s is not either swap chain adequate or the graphics family indices are not complete!",
                 deviceProperties.deviceName.data())
        }

        const char* errMsg = "No suitable device has been found!";
        LOG(Vulkan, Info, errMsg);
        throw std::runtime_error(errMsg);
    }

    bool PhysicalDevice::CheckDeviceExtensionSupport(const vk::PhysicalDevice& physicalDevice,
                                                     std::vector<const char*> requiredExtensions)
    {

        std::vector<vk::ExtensionProperties> extensionProperties = physicalDevice.enumerateDeviceExtensionProperties();

        for (const auto& property : extensionProperties)
        {
            if (requiredExtensions.empty())
                break;

            auto it = std::find_if(requiredExtensions.begin(), requiredExtensions.end(),
                                   [&](const char* const& item) { return strcmp(item, property.extensionName) == 0; });

            if (it != requiredExtensions.end())
            {
                LOGF(Vulkan, Verbose, "Required Extension %s was found", *it)
                requiredExtensions.erase(it);
            }
        }

        return requiredExtensions.empty();
    }

    SwapChainSupportDetails PhysicalDevice::QuerySwapChainSupport(const vk::PhysicalDevice& physicalDevice,
                                                                  const vk::SurfaceKHR& surface)
    {
        SwapChainSupportDetails details;

        details.m_SurfaceInfo.surface = surface;

        details.m_Capabilites = physicalDevice.getSurfaceCapabilitiesKHR(surface);

        uint32_t surfaceFormatCount;
        Utils::CheckVkResult(physicalDevice.getSurfaceFormatsKHR(surface, &surfaceFormatCount, nullptr));

        if (surfaceFormatCount != 0)
        {
            details.m_SurfaceFormats.resize(surfaceFormatCount);
            Utils::CheckVkResult(
                physicalDevice.getSurfaceFormatsKHR(surface, &surfaceFormatCount, details.m_SurfaceFormats.data()));
        }

        details.m_PresentModes = physicalDevice.getSurfacePresentModesKHR(surface);

        return std::move(details);
    }

    vk::Device PhysicalDevice::CreateDevice(const vk::DeviceCreateInfo& deviceCreateInfo) const
    {
        return m_PhysicalDevice.createDevice(deviceCreateInfo);
    }

    SwapChainSupportDetails PhysicalDevice::GetSwapChainSupportDetails() const
    {
        return m_SwapChainDetails;
    }

    QueueFamilyIndices PhysicalDevice::GetQueueFamilyIndices() const
    {
        return m_QueueFamilyIndices;
    }
    vk::FormatProperties PhysicalDevice::GetFormatProperties(const vk::Format format) const
    {
        return m_PhysicalDevice.getFormatProperties(format);
    }

    vk::PhysicalDeviceProperties PhysicalDevice::GetProperties() const
    {
        return m_PhysicalDevice.getProperties();
    }

    vk::PhysicalDeviceMemoryProperties PhysicalDevice::GetMemoryProperties() const
    {
        return m_PhysicalDevice.getMemoryProperties();
    }

    vk::PhysicalDevice& PhysicalDevice::operator*()
    {
        return m_PhysicalDevice;
    }

    QueueFamilyIndices PhysicalDevice::FindQueueFamilyIndices(const vk::PhysicalDevice& physicalDevice,
                                                              const vk::SurfaceKHR& surface)
    {
        QueueFamilyIndices indices{};

        std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

        bool isComplete = false;
        bool isFullyComplete = false;

        for (int i = 0; i < queueFamilyProperties.size(); i++)
        {
            if (isFullyComplete)
            {
                break;
            }

            if (!isComplete)
            {

                if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)
                {
                    indices.m_GraphicsFamily = i;
                }

                VkBool32 presentSupport = physicalDevice.getSurfaceSupportKHR(i, surface);

                if (presentSupport)
                {
                    indices.m_PresentFamily = i;
                }

                isComplete = indices.IsComplete();
            }

            if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eCompute)
            {
                indices.m_ComputeFamily = i;
            }

            if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eTransfer)
            {
                indices.m_TransferFamily = i;
            }

            isFullyComplete = isComplete && indices.HasCompute() && indices.HasTransfer();
        }

        return indices;
    }

    vk::PhysicalDeviceFeatures2 PhysicalDevice::GetPhysicalDeviceFeatures2() const
    {
        return m_PhysicalDevice.getFeatures2();
    }

    vk::PhysicalDeviceLimits PhysicalDevice::GetDeviceLimits() const
    {
		return m_PhysicalDevice.getProperties().limits;
    }

} // namespace VkCore
