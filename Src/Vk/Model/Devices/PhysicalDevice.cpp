#include "PhysicalDevice.h"
#include "../../../Log/Log.h"
#include "../../Utils.h"
#include "../SwapChainSupportDetails.h"

namespace VkCore
{
    PhysicalDevice::PhysicalDevice(const vk::Instance& instance, const vk::SurfaceKHR& surface,
                                   std::set<std::string> requiredExtensions)
    {

        std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();

        if (physicalDevices.size() == 0)
        {
            LOG(Vulkan, Fatal, "None of the physical devices support Vulkan!");
        }

        for (const auto& device : physicalDevices)
        {
            
            // Define it beforehand, so the next operations can be done.
            m_PhysicalDevice = device;

            QueueFamilyIndices indices = FindQueueFamilyIndices(surface);

            bool extensionsSupported = CheckDeviceExtensionSupport(requiredExtensions);

            bool swapChainAdequate = false;
            SwapChainSupportDetails swapChainSupport;

            if (extensionsSupported)
            {
                swapChainSupport = QuerySwapChainSupport(surface);
                swapChainAdequate =
                    !swapChainSupport.m_SurfaceFormats.empty() && !swapChainSupport.m_PresentModes.empty();
            }

            if (indices.isComplete() && extensionsSupported && swapChainAdequate)
            {
                vk::PhysicalDeviceProperties deviceProperties = device.getProperties();

                LOGF(Vulkan, Info, "Suitable physical device has been found. Using %s", deviceProperties.deviceName.data())

                m_QueueFamilyIndices = indices;
                m_PhysicalDevice = device;
                m_SwapChainDetails = swapChainSupport;
                break;
            }
        }
    }

    bool PhysicalDevice::CheckDeviceExtensionSupport(std::set<std::string> requiredExtensions) const
    {

        std::vector<vk::ExtensionProperties> extensionProperties =
            m_PhysicalDevice.enumerateDeviceExtensionProperties();

        for (const auto& extension : extensionProperties)
            requiredExtensions.erase(extension.extensionName);

        return requiredExtensions.empty();
    }

    SwapChainSupportDetails PhysicalDevice::QuerySwapChainSupport(const vk::SurfaceKHR& surface) const
    {
        SwapChainSupportDetails details;

        details.m_SurfaceInfo.surface = surface;

        details.m_Capabilites = m_PhysicalDevice.getSurfaceCapabilitiesKHR(surface);

        uint32_t surfaceFormatCount;
        Utils::CheckVkResult(
            m_PhysicalDevice.getSurfaceFormatsKHR(surface, &surfaceFormatCount, nullptr));

        if (surfaceFormatCount != 0)
        {
            details.m_SurfaceFormats.resize(surfaceFormatCount);
            Utils::CheckVkResult(m_PhysicalDevice.getSurfaceFormatsKHR(surface, &surfaceFormatCount,
                                                                        details.m_SurfaceFormats.data()));
        }

        details.m_PresentModes = m_PhysicalDevice.getSurfacePresentModesKHR(surface);

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

    vk::PhysicalDevice& PhysicalDevice::GetPhysicalDevice()
    {
        return m_PhysicalDevice;
    }

    QueueFamilyIndices PhysicalDevice::FindQueueFamilyIndices(const vk::SurfaceKHR& surface)
    {
        QueueFamilyIndices indices{};

        std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_PhysicalDevice.getQueueFamilyProperties();

        for (int i = 0; i < queueFamilyProperties.size(); i++)
        {
            if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)
            {
                indices.m_GraphicsFamily = i;
            }

            VkBool32 presentSupport = m_PhysicalDevice.getSurfaceSupportKHR(i, surface);

            if (presentSupport)
            {
                indices.m_PresentFamily = i;
            }

            if (indices.isComplete())
                break;
        }

        return indices;
    }

} // namespace VkCore
